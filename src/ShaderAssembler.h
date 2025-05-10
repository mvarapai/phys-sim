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

struct Shader
{
	std::vector<D3D12_INPUT_ELEMENT_DESC>				mInputLayout;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>			mRootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob>					mvsByteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob>					mpsByteCode = nullptr;

	D3D12_INPUT_LAYOUT_DESC GetInputLayoutDesc()
	{
		return { mInputLayout.data(), (UINT)mInputLayout.size() };
	}

	D3D12_SHADER_BYTECODE GetVertexShader()
	{
		return { reinterpret_cast<BYTE*>(mvsByteCode->GetBufferPointer()),
			mvsByteCode->GetBufferSize() };
	}

	D3D12_SHADER_BYTECODE GetPixelShader()
	{
		return { reinterpret_cast<BYTE*>(mpsByteCode->GetBufferPointer()),
			mpsByteCode->GetBufferSize() };
	}

	void Set(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc)
	{
		psoDesc.InputLayout = GetInputLayoutDesc();
		psoDesc.pRootSignature = mRootSignature.Get();
		psoDesc.VS = GetVertexShader();
		psoDesc.PS = GetPixelShader();
	}
};
