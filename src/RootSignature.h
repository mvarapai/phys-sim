/*****************************************************************//**
 * \file   RootSignature.h
 * \brief  Defines root signatures in use by the application.
 * 
 * \author Mikalai Varapai
 * \date   May 2025
 *********************************************************************/

// The principle is:
// We define base wrapper virtual class RootSignature, and then create classes for
// every single root signature, creating it uniquely in the constructor,
// and implementing the SetRootParameters(..) method that accepts generic structures.

#pragma once

#include <d3d12.h>
#include <wrl.h>
#include "d3dUtil.h"

// DEFINES

#define MAX_TEXTURES_PER_CALL 16

// Root signature wrapper class. Does nothing by itself.
class RootSignature
{
protected:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSig = nullptr;

public:

	RootSignature() = default;
	void SetRootSignature(ID3D12GraphicsCommandList* pCmdList, ID3D12DescriptorHeap** ppDescHeapArray, UINT numDescHeaps)
	{
		pCmdList->SetGraphicsRootSignature(mRootSig.Get());

		if (numDescHeaps > 0)
			pCmdList->SetDescriptorHeaps(numDescHeaps, ppDescHeapArray);
	}
	virtual void SetRootParametersPerDraw(ID3D12GraphicsCommandList* pCmdList, void* pParams, UINT paramByteLength) = 0;
	virtual void SetRootParametersPerPass(ID3D12GraphicsCommandList* pCmdList, void* pParams, UINT paramByteLength) = 0;
};

// b0		: per pass CBV
// b1		: object transform matrix CBV
// b2		: material CBV
// t(0..15) : texture SRV slots
class RootSignature_Default : public RootSignature
{
public:
	struct PerPass
	{
		D3D12_GPU_VIRTUAL_ADDRESS passCB;
	};
	struct PerDraw
	{
		D3D12_GPU_VIRTUAL_ADDRESS objectCB;
		D3D12_GPU_VIRTUAL_ADDRESS dynamicMaterialCB;
		D3D12_GPU_DESCRIPTOR_HANDLE srvTextureBase;
	};
public:
	RootSignature_Default(ID3D12Device* pDevice)
	{
		// Root parameter can be a table, root descriptor or root constants.
		D3D12_ROOT_PARAMETER slotRootParameters[4] = { };

		// Pass CBV will be bound to b0
		D3D12_ROOT_DESCRIPTOR perPassCBV = { };
		perPassCBV.RegisterSpace = 0;
		perPassCBV.ShaderRegister = 0;

		D3D12_ROOT_DESCRIPTOR perObjectCBV = { };
		perObjectCBV.RegisterSpace = 0;
		perObjectCBV.ShaderRegister = 1;

		D3D12_ROOT_DESCRIPTOR materialCBV = { };
		materialCBV.RegisterSpace = 0;
		materialCBV.ShaderRegister = 2;

		D3D12_ROOT_DESCRIPTOR_TABLE srvTable = { };

		D3D12_DESCRIPTOR_RANGE srvDescriptorRange = { };
		srvDescriptorRange.BaseShaderRegister = 0;
		srvDescriptorRange.NumDescriptors = MAX_TEXTURES_PER_CALL;
		srvDescriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		srvDescriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		srvDescriptorRange.RegisterSpace = 0;

		srvTable.pDescriptorRanges = &srvDescriptorRange;
		srvTable.NumDescriptorRanges = 1;

		slotRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		slotRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		slotRootParameters[0].Descriptor = perPassCBV;

		slotRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		slotRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		slotRootParameters[1].Descriptor = perObjectCBV;

		slotRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		slotRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		slotRootParameters[2].Descriptor = materialCBV;

		slotRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		slotRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		slotRootParameters[3].DescriptorTable = srvTable;

		// Create static samplers

		D3D12_STATIC_SAMPLER_DESC samplerDesc = { };
		samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
		samplerDesc.RegisterSpace = 0;
		samplerDesc.ShaderRegister = 0;
		samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		// Create root signature description
		D3D12_ROOT_SIGNATURE_DESC rootDesc = { };
		rootDesc.NumParameters = _countof(slotRootParameters);
		rootDesc.pParameters = slotRootParameters;
		rootDesc.NumStaticSamplers = 1;
		rootDesc.pStaticSamplers = &samplerDesc;
		rootDesc.Flags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		// Create a root signature with a single slot which points to
		// a descriptor range consisting of a single constant buffer
		Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
		HRESULT hr = D3D12SerializeRootSignature(&rootDesc,
			D3D_ROOT_SIGNATURE_VERSION_1,
			serializedRootSig.GetAddressOf(),
			errorBlob.GetAddressOf());

		// Error handling
		if (errorBlob != nullptr)
		{
			::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		}
		ThrowIfFailed(hr);

		// Create the root signature
		ThrowIfFailed(pDevice->CreateRootSignature(0,
			serializedRootSig->GetBufferPointer(),
			serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(mRootSig.GetAddressOf())));
	}

	void SetRootParametersPerPass(ID3D12GraphicsCommandList* pCmdList, void* pParams, UINT paramByteLength) override
	{
		if (paramByteLength != sizeof(PerPass)) return;

		PerPass* mem = reinterpret_cast<PerPass*>(pParams);
		pCmdList->SetGraphicsRootConstantBufferView(0, mem->passCB);
	}

	void SetRootParametersPerDraw(ID3D12GraphicsCommandList* pCmdList, void* pParams, UINT paramByteLength) override
	{
		if (paramByteLength != sizeof(PerDraw)) return;

		PerDraw* mem = reinterpret_cast<PerDraw*>(pParams);

		pCmdList->SetGraphicsRootConstantBufferView(1, mem->objectCB);
		pCmdList->SetGraphicsRootConstantBufferView(2, mem->dynamicMaterialCB);
		pCmdList->SetGraphicsRootDescriptorTable(3, mem->srvTextureBase);
	}
};
