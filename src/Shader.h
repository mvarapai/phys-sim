/*****************************************************************//**
 * \file   ShaderAssembler.h
 * \brief  Loads and caches the shader.
 * 
 * \author Mikalai Varapai
 * \date   May 2025
 *********************************************************************/

#pragma once

#include <vector>
#include <d3d12.h>
#include <wrl.h>
#include <type_traits>

#include "PredefinedVertexStructures.h"

// The intended usage is as follows:
//	1. Resource manager class stores instances of class Shader.
//	2. Then, these instances are initialized with ShaderBase<VertexType> by
//		the resource initializer. In constructor, VertexBase exposes pointers
//		to underlying bytecode, which are to be initialized by the caller,
//		although this class maintains its ownership.

class Shader
{
protected:
	ID3D12RootSignature*								pRootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob>					bytecode = nullptr;

	virtual D3D12_INPUT_LAYOUT_DESC GetInputLayoutDesc() = 0;

	D3D12_SHADER_BYTECODE GetBytecode()
	{
		return { reinterpret_cast<BYTE*>(bytecode->GetBufferPointer()),
			mvsByteCode->GetBufferSize() };
	}

public:
	void Set(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc)
	{
		psoDesc.InputLayout = GetInputLayoutDesc();
		psoDesc.pRootSignature = pRootSignature;
		psoDesc.VS = GetVertexShader();
		psoDesc.PS = GetPixelShader();
	}
};

template <typename T>
class ShaderBase : public Shader
{
private:
	ShaderBase(ID3D12RootSignature* pRootSignature,
		ID3D12Blob** &ppVSBytecode,
		ID3D12Blob** &ppPSBytecode) : pRootSignature(pRootSignature)
	{
		// Assert if T is a vertex type
		static_assert(std::is_base_of_v<VertexBase, T>,
			"Shader must only use a vertex type.");

		ppVSBytecode = mvsByteCode.GetAddressOf();
		ppPSBytecode = mpsByteCode.GetAddressOf();
	}

	D3D12_INPUT_LAYOUT_DESC GetInputLayoutDesc() override { return T::InputLayoutDesc; }
};
