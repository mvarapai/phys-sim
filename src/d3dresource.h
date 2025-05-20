/*****************************************************************//**
 * \file   d3dresource.h
 * \brief  Describes static and dynamic resource structure
 * 
 * \author 20231063
 * \date   June 2024
 *********************************************************************/

#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <ResourceUploadBatch.h>
#include <DDSTextureLoader.h>

#include "structures.h"
#include "geometry.h"
#include "RingBuffer.h"

#define CBUFFER_MAX_SIZE 64

#define NUM_FRAME_RESOURCES 3

class ConstantBuffer
{
public:
	std::unordered_map<std::string, std::unique_ptr<ConstantBuffer>>& ConstantBufferDictionary()
	{
		static std::unordered_map<std::string, std::unique_ptr<ConstantBuffer>> map;
		return map;
	}

protected:
	static int currentFrameResource;
public:
	virtual D3D12_GPU_VIRTUAL_ADDRESS GetBufferAddress() = 0;
};

class MaterialsConstantBuffer : ConstantBuffer
{
public:
	
private:
	std::unique_ptr<RingBuffer<MaterialConstants, CBUFFER_MAX_SIZE>> Buffers[NUM_FRAME_RESOURCES];
};

struct ConstantBufferDataCPU
{
	std::vector<ObjectConstants> ObjectTransforms;

	PassConstants PassBuffer = { };

	MaterialConstants Materials[NUM_MATERIALS];
	int MaterialModified[NUM_MATERIALS];			// Number dirty frames


	// Initialize CPU memory
	ConstantBufferDataCPU(std::vector<ObjectConstants>& transformInitialData, MaterialConstants* pMaterialInitialData)
	{
		// Copy the object transform vector
		ObjectTransforms = transformInitialData;
		
		for (int i = 0; i < NUM_MATERIALS; i++)
		{
			Materials[i] = pMaterialInitialData[i];
			MaterialModified[i] = NUM_FRAME_RESOURCES;
		}
	}
};

class FrameResourceManager
{
	struct FrameResource
	{
		FrameResource(ID3D12Device* pDevice, 
			std::vector<ObjectConstants>& objCPU,
			std::vector<MaterialConstants>& materialCPU)
		{
			pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(CommandListAllocator.GetAddressOf()));

			PassCB = std::make_unique<UploadBuffer<PassConstants>>(pDevice, passCount, true);
			ObjectCB = std::make_unique<RingBuffer<ObjectConstants, CBUFFER_MAX_SIZE>>(pDevice, true, objCPU);
			MaterialCB = std::make_unique<UploadBuffer<MaterialConstants>>(pDevice, materialCount, true);
		}

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator>		CommandListAllocator = nullptr;

		std::unique_ptr<UploadBuffer<PassConstants>>						PassCB = nullptr;
		std::unique_ptr<RingBuffer<ObjectConstants, CBUFFER_MAX_SIZE>>		ObjectCB = nullptr;
		std::unique_ptr<RingBuffer<MaterialConstants, CBUFFER_MAX_SIZE>>	MaterialCB = nullptr;

		UINT64 Fence = 0;
	};

	std::unique_ptr<FrameResource> pFrameResources[NUM_FRAME_RESOURCES];
	UINT currFrameResourceIndex = 0;

	ConstantBufferDataCPU CBDataCPU;


public:
	FrameResource* pCurrentFrameResource = nullptr;

	DynamicResources(ID3D12Device* pDevice, 
		std::vector<ObjectConstants> pTransformInitialData, MaterialConstants* pMaterialInitialData)
		: CBDataCPU(pTransformInitialData, pMaterialInitialData)
	{
		for (int i = 0; i < NUM_FRAME_RESOURCES; i++)
		{
			pFrameResources[i] =
				std::make_unique<FrameResource>(pDevice, 1, NUM_MATERIALS, CBDataCPU.ObjectTransforms);
		}
		pCurrentFrameResource = pFrameResources[currFrameResourceIndex].get();
	}

	void NextFrameResource(ID3D12Fence* pFence)
	{
		// Write to next frame resource
		currFrameResourceIndex =
			(currFrameResourceIndex + 1) % NUM_FRAME_RESOURCES;
		pCurrentFrameResource = pFrameResources[currFrameResourceIndex].get();

		// Check whether the GPU has finished processing current frame
		if (pCurrentFrameResource->Fence != 0
			&& pFence->GetCompletedValue() < pCurrentFrameResource->Fence)
		{
			HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
			ThrowIfFailed(pFence->SetEventOnCompletion(pCurrentFrameResource->Fence, eventHandle));
			if (eventHandle == nullptr) return;
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}

	void UpdateConstantBuffers()
	{
		// Reset the object CB. Object transform data are uploaded dynamically
		// through the usage of GetGPUHandle()
		pCurrentFrameResource->ObjectCB->FrameReset();

		// Update GPU buffer for pass constants

		pCurrentFrameResource->PassCB->CopyData(0,
			CBDataCPU.PassBuffer);

		// Update GPU buffer for material constants

		for (UINT i = 0; i < NUM_MATERIALS; i++)
		{
			if (CBDataCPU.MaterialModified[i] > 0)
			{
				pCurrentFrameResource->MaterialCB->CopyData(i,
					CBDataCPU.Materials[i]);

				CBDataCPU.MaterialModified[i]--;
			}
		}
	}

	// Handles to retrieve and change CB data

	void SetObjectTransform(UINT index, const ObjectConstants& transform)
	{
		CBDataCPU.ObjectTransforms[index] = transform;
	}

	// We let application do pass constants assignment
	void SetPassConstants(const PassConstants& pass)
	{
		CBDataCPU.PassBuffer = pass;
	}

	void SetMaterial(UINT index, const MaterialConstants& material)
	{
		CBDataCPU.Materials[index] = material;
		CBDataCPU.MaterialModified[index] = NUM_FRAME_RESOURCES;
	}

	ObjectConstants GetTransform(UINT index) { return CBDataCPU.ObjectTransforms[index]; }
	PassConstants GetPassConstants() { return CBDataCPU.PassBuffer; }
	MaterialConstants GetMaterialConstants(UINT index) { return CBDataCPU.Materials[index]; }

};
