/*****************************************************************//**
 * \file   ringbuffer.cpp
 * \brief  Define constant buffer structure to allow any number of
 *		   drawables to be rendered.
 * 
 * \author Mikalai Varapai
 * \date   May 2025
 *********************************************************************/

#include <vector>
#include <wrl.h>

#include "UploadBuffer.h"

// Ring buffer is just a subset of upload buffer, where we
// reupload data after its memory segment was used.
//
// Ring buffer creates an illusion that we use continuous index range, while in reality we just
// give it an offsetted memory index, allowing for dynamic memory allocation and usage.
// Uses the sequential property of the rendering application: elements loaded earlier will
// be guaranteed processed before the ones loaded later.
template<typename T, int batchSize>
struct RingBuffer : UploadBuffer<T>
{
public:
	RingBuffer(ID3D12Device* pDevice, bool isConstantBuffer, std::vector<T>& cpuData)
		: UploadBuffer<T>(pDevice, batchSize, isConstantBuffer), elementsCPU(cpuData)
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
			return elementsCPU.push_back(T);

		elementsCPU[index] = T;
	}

	// Reset in the beginning of every frame
	void FrameReset()
	{
		nextSlotIndex = 0;
		std::memset(slotFenceValues, 0, sizeof(slotFenceValues));
	}

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUHandle(int index)
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

		nextSlotIndex = (nextSlotIndex + 1) % batchSize;
		return addr;
	}

private:
	// Synchronization constraints
	Microsoft::WRL::ComPtr<ID3D12Fence> pFence = nullptr;	// Unique fence for every instance

	UINT64 slotFenceValues[batchSize] = { };
	int nextSlotIndex = 0;			// In range [0, 16), as opposed to virtual indices of std::vector
	std::vector<T>& elementsCPU;	// The exact point of using ring buffer: ultimately it's just a simulation of std::vector
};
