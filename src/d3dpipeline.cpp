/*****************************************************************//**
 * \file   d3dpipeline.cpp
 * \brief  Handles PSOs and shaders
 * 
 * \author Mikalai Varapai
 * \date   November 2023
 *********************************************************************/

#include <d3d12.h>
#include <DDSTextureLoader.h>
#include <ResourceUploadBatch.h>

#include "d3dinit.h"
#include "d3dUtil.h"
#include "d3dapp.h"

using Microsoft::WRL::ComPtr;

void D3DApplication::LoadResources()
{
	// LOAD RESOURCES
	pStaticResources = std::make_unique<StaticResources>();
	pStaticResources->LoadGeometry(md3dDevice.Get(), mCommandQueue.Get(),
		mFence.Get(), mCurrentFence);
	pStaticResources->LoadTextures(md3dDevice.Get(), mCommandQueue.Get());

	// Set materials and transforms

	MaterialConstants materials[NUM_MATERIALS];
	// Grass
	materials[0].DiffuseAlbedo = DirectX::XMFLOAT4(0.0f, 0.6f, 0.0f, 1.0f);
	materials[0].FresnelR0 = DirectX::XMFLOAT3(0.01f, 0.01f, 0.01f);
	materials[0].Roughness = 0.8f;
	materials[0].MatTransform = MathHelper::Identity4x4();

	materials[1].DiffuseAlbedo = DirectX::XMFLOAT4(0.0f, 0.2f, 0.6f, 0.5f);
	materials[1].FresnelR0 = DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f);
	materials[1].Roughness = 0.0f;
	materials[1].MatTransform = MathHelper::Identity4x4();

	std::vector<ObjectConstants> objectTransforms(2, { MathHelper::Identity4x4() });

	//XMMATRIX terrain = XMMatrixIdentity();
	//terrain *= XMMatrixTranslation(0.0f, -4.0f, 0.0f);
	//XMStoreFloat4x4(&objects[0].World, terrain);

	pDynamicResources = std::make_unique<DynamicResources>(md3dDevice.Get(), objectTransforms, materials);

	mTerrain = std::make_unique<DefaultDrawable>(
		pStaticResources->Geometries[0].Submeshes.at(0), 0, 0, pStaticResources->GetTextureSRV(0));

	mWater = std::make_unique<DefaultDrawable>(
		pStaticResources->Geometries[0].Submeshes.at(1), 1, 1, pStaticResources->GetTextureSRV(1));

}

// Compile shaders and create input layout
void D3DApplication::BuildShadersAndInputLayout()
{
	HRESULT hr = S_OK;

	const D3D_SHADER_MACRO defines[] =
	{
		"FOG", "1",
		NULL, NULL
	};

	mDefaultShader.mvsByteCode = CompileShader(L"resources\\Shaders\\main.hlsl",
		defines, "VS", "vs_5_0");
	mDefaultShader.mpsByteCode = CompileShader(L"resources\\Shaders\\main.hlsl",
		defines, "PS", "ps_5_0");

	mDefaultShader.mInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
}

void D3DApplication::BuildPSO()
{
	// Rasterizer desc

	D3D12_RASTERIZER_DESC rd = D3DHelper::DefaultRasterizerDesc();
	D3D12_BLEND_DESC bd = D3DHelper::DefaultBlendDescOff();
	D3D12_DEPTH_STENCIL_DESC dsd = D3DHelper::DefaultDepthStencilDesc();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = { };
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	// Set shaders
	mDefaultShader.Set(psoDesc);
	
	psoDesc.RasterizerState = rd;
	psoDesc.BlendState = bd;
	psoDesc.DepthStencilState = dsd;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = mBackBufferFormat;
	psoDesc.SampleDesc.Count = msaaEnabled ? 4 : 1;
	psoDesc.SampleDesc.Quality = msaaEnabled ? (msaaQualityLevels - 1) : 0;
	psoDesc.DSVFormat = mDepthStencilFormat;

	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(
		&psoDesc, IID_PPV_ARGS(mDefaultPSO.GetAddressOf())));

	// Create another PSO for line list drawing
	D3D12_GRAPHICS_PIPELINE_STATE_DESC linePSODesc = psoDesc;
	linePSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(
		&linePSODesc, IID_PPV_ARGS(mLinePSO.GetAddressOf())));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC blendingPSO = psoDesc;

	D3D12_RENDER_TARGET_BLEND_DESC blendDesc = { };

	blendDesc.BlendEnable = TRUE;
	blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	blendDesc.LogicOpEnable = FALSE;
	blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;

	blendingPSO.BlendState.RenderTarget[0] = blendDesc;

	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(
		&blendingPSO, IID_PPV_ARGS(mBlendPSO.GetAddressOf())));
}