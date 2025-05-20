// **************************************************************************
//							UploadBuffer.h									*
//																			*
//	Wrapper class for creating a buffer resource on the upload heap,		*
//	so that it can be modified by the CPU during runtime.					*
//																			*
//	Constructor creates commited resource with upload heap and maps its		*
//	contents to the class member with pointer to the data.					*
//																			*
//	CopyData(int elementIndex, const T& data) - copies contents of T		*
//	to the [elementIndex] entry to the buffer, using CB padding if necessary*
//	Is used to copy only one element, for an array an overload can be used.	*
//																			*
// **************************************************************************

#pragma once

#include <wrl.h>

#include "d3dUtil.h"

/**
 * Class-wrapper for GPU upload buffer.
 *
 * Usage:
 *	Create through constructor
 */
template<typename T>
class UploadBuffer
{
public:
	// Constructor for creating upload buffer
	UploadBuffer(ID3D12Device* device, UINT elementCount, bool isConstantBuffer)
	{
		// Consider general non-CB case
		mElementByteSize = sizeof(T);

		// Constant buffer elements need to be a multiple of 256 bytes.
		// This is because the compiler can only view constant data
		// at m * 256 byte offsets and of n * 256 byte lengths.
		if (isConstantBuffer)
			mElementByteSize = CalcConstantBufferByteSize(sizeof(T));

		// Memory allocation info
		D3D12_HEAP_PROPERTIES hp = HeapProperties(D3D12_HEAP_TYPE_UPLOAD);

		// Upload buffer description
		D3D12_RESOURCE_DESC bufferDesc = BufferDesc(static_cast<UINT64>(mElementByteSize) * elementCount);

		// Create upload buffer and commit it to the GPU heap
		ThrowIfFailed(device->CreateCommittedResource(
			&hp,								// Upload heap properties
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,						// Resource description
			D3D12_RESOURCE_STATE_COMMON,		// Initial state
			nullptr,
			IID_PPV_ARGS(mUploadBuffer.GetAddressOf())));

		// Get pointer to the underlying memory
		// 0, nullptr means that we map the entire resource
		// mMappedData contains pointer to contents of the buffer,
		// where we can upload during runtime
		ThrowIfFailed(mUploadBuffer->Map(0, nullptr,
			reinterpret_cast<void**>(&mMappedData)));

		// We do not unmap until the destructor is called.
		// However, we must not write to the resource while it is in
		// use by the GPU, therefore we should use synchronization
	}

	// Forbid copying
	UploadBuffer(UploadBuffer& rhs) = delete;
	UploadBuffer& operator=(const UploadBuffer& rhs) = delete;

	// Unmap data upon destruction
	~UploadBuffer()
	{
		if (mUploadBuffer != nullptr)
		{
			mUploadBuffer->Unmap(0, nullptr);
		}
		mMappedData = nullptr;
	}

	// Getter for ID3D12Resource interface
	ID3D12Resource* Resource() const { return mUploadBuffer.Get(); }

	// Copy given element to the buffer at [elementIndex] slot
	void CopyData(int elementIndex, const T& data)
	{
		memcpy(&mMappedData[elementIndex * mElementByteSize],
			&data, sizeof(T));
	}

	// Convenience method to copy an array of data, including CB padding
	void CopyData(int firstElementIndex, int numElements, const T* dataArray)
	{
		for (int i = 0; i < numElements; i++)
		{
			CopyData(firstElementIndex + i, dataArray[i]);
		}
	}

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUHandle(int index)
	{
		D3D12_GPU_VIRTUAL_ADDRESS base = mUploadBuffer->GetGPUVirtualAddress();
		base += mElementByteSize * index;
		return base;
	}

private:
	// Pointer to underlying GPU resource
	// Is released automatically
	Microsoft::WRL::ComPtr<ID3D12Resource> mUploadBuffer = nullptr;

	// Pointer to data contained in the buffer
	// It is mapped upon creation and unmapped when destroyed
	BYTE* mMappedData = nullptr;

	// Byte size of one element of an upload buffer
	// Is padded to multiple of 256 bytes in case of constant buffer
	UINT mElementByteSize = 0;

	// 
	bool mIsConstantBuffer = false;
};

// Ring buffer is just a subset of upload buffer, where we
// reupload data after its memory segment was used.
//
// Ring buffer creates an illusion that we use continuous index range, while in reality we just
// give it an offsetted memory index, allowing for dynamic memory allocation and usage.
// Uses the sequential property of the rendering application: elements loaded earlier will
// be guaranteed processed before the ones loaded later.
template<typename T, size_t BatchSize>
struct RingBuffer : public UploadBuffer<T>
{
public:
	RingBuffer(ID3D12Device* pDevice, bool isConstantBuffer, std::vector<T>& cpuData)
		: UploadBuffer<T>(pDevice, BatchSize, isConstantBuffer), elementsCPU(cpuData)
	{
		// Create the fence with initial value 0
		ThrowIfFailed(pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(pFence.GetAddressOf())));
	}

	// Reset the whole CPU data array
	void SetCPUData(std::vector<T>& data)
	{
		elementsCPU = data;
	}

	// Update particular element
	void UpdateElementCPU(T& elem, int index)
	{
		if (index >= elementsCPU)
			return elementsCPU.push_back(elem);

		elementsCPU[index] = elem;
	}

	void Reset()
	{
		nextSlotIndex = 0;
		std::memset(slotFenceValues, 0, sizeof(slotFenceValues));
	}

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUHandleSingle(size_t index)
	{
		// Wait for next slot to become free
		if (pFence->GetCompletedValue() < slotFenceValues[nextSlotIndex])
		{
			HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
			ThrowIfFailed(pFence->SetEventOnCompletion(slotFenceValues[nextSlotIndex], eventHandle));
			if (eventHandle == nullptr) return 0;
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}

		// Copy data from function argument to next slot position, as it is safe now
		UploadBuffer<T>::CopyData(nextSlotIndex, elementsCPU.at(index));
		D3D12_GPU_VIRTUAL_ADDRESS addr = UploadBuffer<T>::GetGPUHandle(nextSlotIndex);

		// Advance fence value and set next slot
		slotFenceValues[nextSlotIndex] = currentFenceValue;
		pFence->Signal(currentFenceValue);
		currentFenceValue++;
		nextSlotIndex = (nextSlotIndex + 1) % BatchSize;

		return addr;
	}

	// Same as single, but allocated a range
	D3D12_GPU_VIRTUAL_ADDRESS GetGPUHandleRange(size_t index, size_t numElements)
	{
		// Cannot allocate more elements than BatchSize
		if (numElements > BatchSize) return 0;

		int lastSlotIndex = (nextSlotIndex - 1) + numElements;
		lastSlotIndex %= BatchSize;

		// Wait for last slot to become free
		if (pFence->GetCompletedValue() < slotFenceValues[lastSlotIndex])
		{
			HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
			ThrowIfFailed(pFence->SetEventOnCompletion(slotFenceValues[nextSlotIndex], eventHandle));
			if (eventHandle == nullptr) return 0;
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}

		// Copy data from function argument to next slot position, as it is safe now
		UploadBuffer<T>::CopyData(nextSlotIndex, numElements, (elementsCPU.data() + index));
		D3D12_GPU_VIRTUAL_ADDRESS addr = UploadBuffer<T>::GetGPUHandle(nextSlotIndex);

		// Advance fence value and set next slot.
		// It only really matters that the first element's fence value
		// is changed as once we are done with the first element, we are done with all of them.
		slotFenceValues[nextSlotIndex] = currentFenceValue;
		pFence->Signal(currentFenceValue);
		currentFenceValue++;
		nextSlotIndex = (lastSlotIndex + 1) % BatchSize;

		return addr;
	}

private:
	// Synchronization constraints
	Microsoft::WRL::ComPtr<ID3D12Fence> pFence = nullptr;	// Unique fence for every instance

	UINT64 currentFenceValue = 1;
	UINT64 slotFenceValues[BatchSize] = { };
	int nextSlotIndex = 0;			// In range [0, BatchSize), as opposed to virtual indices of std::vector
	std::vector<T>& elementsCPU;	// The exact point of using ring buffer: ultimately it's just a simulation of std::vector
};
