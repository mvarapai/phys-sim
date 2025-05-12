/*****************************************************************//**
 * \file   PipelineState.h
 * \brief  Defines PSO creation, loading and caching.
 * 
 * \author Mikalai Varapai
 * \date   May 2025
 *********************************************************************/

#pragma once

#include <string>
#include <d3d12.h>
#include <vector>
#include <unordered_map>

#include "DebugPrint.h"

// Structure describing PSO from file.
struct PSO_DESC
{
	std::string name;

	std::string rootSignature;

	std::string VS;
	std::string PS;
	std::string HS;
	std::string DS;
	std::string GS;

	std::string vertexType;					// D3D12_INPUT_LAYOUT_DESC 
	std::string primitiveTopologyType;		// D3D12_PRIMITIVE_TOPOLOGY_TYPE

	// Rasterizer state
	std::string fillMode;					// D3D12_FILL_NODE
	std::string cullMode;					// D3D12_CULL_MODE
	BOOL frontCounterClockwise;
	INT depthBias;
	FLOAT depthBiasClamp;
	FLOAT slopeScaledDepthBias;
	BOOL depthClipEnable;
	BOOL multisampleEnable;
	BOOL antialiasedLineEnable;
	INT forcedSampleCount;
	std::string conservativeRaster;			// D3D12_CONSERVATIVE_RASTERIZATION_MODE

	// Blend state
	BOOL alphaToCoverageEnable;
	BOOL independentBlendEnable;
	struct BLEND_STATE_RENDER_TARGET_DESC
	{
		BOOL blendEnable;
		BOOL logicOpEnable;
		std::string srcBlend;				// D3D12_BLEND
		std::string destBlend;				// D3D12_BLEND
		std::string blendOp;				// D3D12_BLEND_OP
		std::string srcBlendAlpha;			// D3D12_BLEND
		std::string destBlendAlpha;			// D3D12_BLEND
		std::string blendOpAlpha;			// D3D12_BLEND_OP
		std::string logicOp;				// D3D12_LOGIC_OP
		UINT8 renderTargetWriteMask;
	};
	// D3D12_RENDER_TARGET_BLEND_DESC
	std::vector<BLEND_STATE_RENDER_TARGET_DESC> blendStateRenderTargets;

	// Depth stencil state
	BOOL depthEnable;
	std::string depthWriteMask;				// D3D12_DEPTH_WRITE_MASK
	std::string depthFunc;					// D3D12_COMPARISON_FUNC
	BOOL stencilEnable;
	UINT8 stencilReadMask;
	UINT8 stencilWriteMask;

	// 2x D3D12_DEPTH_STENCILOP_DESC
	std::string stencilFailOpFront;			// D3D12_STENCIL_OP
	std::string stencilDepthFailOpFront;	// D3D12_STENCIL_OP
	std::string stencilPassOpFront;			// D3D12_STENCIL_OP
	std::string stencilFuncFront;			// D3D12_COMPARISON_FUNC
	std::string stencilFailOpBack;			// D3D12_STENCIL_OP
	std::string stencilDepthFailOpBack;		// D3D12_STENCIL_OP
	std::string stencilPassOpBack;			// D3D12_STENCIL_OP
	std::string stencilFuncBack;			// D3D12_COMPARISON_FUNC

	UINT sampleMask;
	UINT numRenderTargets;
	std::vector<std::string> rtvFormats;	// DXGI_FORMAT[]
	std::string dsvFormat;					// DXGI_FORMAT
	UINT sampleCount;
	UINT sampleQuality;

	UINT nodeMask;
	int cachedPSOSizeInBytes;
	int blobData;

	int flags;								// D3D12_PIPELINE_STATE_FLAGS
};

class PipelineState
{
public:
	static std::unordered_map<std::string, std::unique_ptr<PipelineState>>& PipelineStateDictionary()
	{
		static std::unordered_map<std::string, std::unique_ptr<PipelineState>> map;
		return map;
	}

private:
	static Microsoft::WRL::ComPtr<ID3D12PipelineLibrary1> PSOLibrary;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> PSO = nullptr;

	PipelineState(ID3D12PipelineState**& ppPipelineState) 
	{
		ppPipelineState = PSO.GetAddressOf();
	}

	friend class PipelineStateAssembler;
};

class PipelineStateAssembler
{
	static int psoIndex;	// Initialized 0
	
#ifdef _DEBUG | DEBUG
	static constexpr bool Optimize = false;
#else
	static constexpr bool Optimize = true;
#endif

public:
	static void InitializePipelineState(const PSO_DESC& desc, ID3D12Device* device)
	{
		// Retrieve ID3D12Device1 interface
		Microsoft::WRL::ComPtr<ID3D12Device1> pDevice = nullptr;
		HRESULT hr = device->QueryInterface(IID_PPV_ARGS(pDevice.GetAddressOf()));
		if (FAILED(hr))
		{
			DPRINT_LOC("ERROR: COULD NOT RETRIEVE DEVICE VERSION 1");
			std::exit(1);
		}

		// Paths
		std::wstring path = L"resources\\pso\\";
		path += L"pso_cache.bin";


		// We create a PSO for the first time, load pipeline library
		if (psoIndex == 0)
		{
			// Load the PSO cache only if optimizations are allowed.
			Microsoft::WRL::ComPtr<ID3DBlob> cacheBlob = nullptr;
			if (Optimize) D3DReadFileToBlob(path.c_str(), cacheBlob.GetAddressOf());

			// Two options possible: blob contains data or is nullptr.
			hr = pDevice->CreatePipelineLibrary(
				cacheBlob ? cacheBlob->GetBufferPointer() : nullptr,
				cacheBlob ? cacheBlob->GetBufferSize() : 0,
				IID_PPV_ARGS(PipelineState::PSOLibrary.GetAddressOf()));

			if (FAILED(hr))
			{
				DPRINT_LOC("ERROR: COULD NOT CREATE PSO LIBRARY");
				std::exit(0);
			}

			// Now we are either way with the library - whether empty or not
		}

		// Create the description

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = { };

		psoDesc.pRootSignature = RootSignature::RootSignatureDictionary()[desc.rootSignature].get()->mRootSig.Get();

		psoDesc.VS = Shader::GetBytecode(desc.VS);
		psoDesc.PS = Shader::GetBytecode(desc.PS);
		psoDesc.HS = Shader::GetBytecode(desc.HS);
		psoDesc.DS = Shader::GetBytecode(desc.DS);
		psoDesc.GS = Shader::GetBytecode(desc.GS);

		psoDesc.InputLayout = GetInputLayout(desc.vertexType);

		psoDesc.PrimitiveTopologyType = StringToPrimitiveTopologyType(desc.primitiveTopologyType);

		psoDesc.RasterizerState.FillMode = StringToFillMode(desc.fillMode);
		psoDesc.RasterizerState.CullMode = StringToCullMode(desc.cullMode);
		psoDesc.RasterizerState.FrontCounterClockwise = desc.frontCounterClockwise;
		psoDesc.RasterizerState.DepthBias = desc.depthBias;
		psoDesc.RasterizerState.DepthBiasClamp = desc.depthBiasClamp;
		psoDesc.RasterizerState.SlopeScaledDepthBias = desc.slopeScaledDepthBias;
		psoDesc.RasterizerState.DepthClipEnable = desc.depthClipEnable;
		psoDesc.RasterizerState.MultisampleEnable = desc.multisampleEnable;
		psoDesc.RasterizerState.AntialiasedLineEnable = desc.antialiasedLineEnable;
		psoDesc.RasterizerState.ForcedSampleCount = desc.forcedSampleCount;
		psoDesc.RasterizerState.ConservativeRaster = StringToRasterizationMode(desc.conservativeRaster);
		
		psoDesc.BlendState.AlphaToCoverageEnable = desc.alphaToCoverageEnable;
		psoDesc.BlendState.IndependentBlendEnable = desc.independentBlendEnable;

		int i = 0;
		for (const auto& rt : desc.blendStateRenderTargets)
		{
			psoDesc.BlendState.RenderTarget[i].BlendEnable = rt.blendEnable;
			psoDesc.BlendState.RenderTarget[i].LogicOpEnable = rt.logicOpEnable;
			psoDesc.BlendState.RenderTarget[i].SrcBlend = StringToBlend(rt.srcBlend);
			psoDesc.BlendState.RenderTarget[i].DestBlend = StringToBlend(rt.destBlend);
			psoDesc.BlendState.RenderTarget[i].BlendOp = StringToBlendOp(rt.blendOp);
			psoDesc.BlendState.RenderTarget[i].SrcBlendAlpha = StringToBlend(rt.srcBlendAlpha);
			psoDesc.BlendState.RenderTarget[i].DestBlendAlpha = StringToBlend(rt.destBlendAlpha);
			psoDesc.BlendState.RenderTarget[i].BlendOpAlpha = StringToBlendOp(rt.blendOpAlpha);
			psoDesc.BlendState.RenderTarget[i].LogicOp = StringToLogicOp(rt.logicOp);
			psoDesc.BlendState.RenderTarget[i].RenderTargetWriteMask = rt.renderTargetWriteMask;

			i++;
		}

		psoDesc.DepthStencilState.DepthEnable = desc.depthEnable;
		psoDesc.DepthStencilState.DepthWriteMask = StringToDepthWriteMask(desc.depthWriteMask);
		psoDesc.DepthStencilState.DepthFunc = StringToComparisonFunc(desc.depthFunc);
		psoDesc.DepthStencilState.StencilEnable = desc.stencilEnable;
		psoDesc.DepthStencilState.StencilReadMask = desc.stencilReadMask;
		psoDesc.DepthStencilState.StencilWriteMask = desc.stencilWriteMask;

		psoDesc.DepthStencilState.FrontFace.StencilFailOp = StringToStencilOp(desc.stencilDepthFailOpFront);
		psoDesc.DepthStencilState.FrontFace.StencilDepthFailOp = StringToStencilOp(desc.stencilDepthFailOpFront);
		psoDesc.DepthStencilState.FrontFace.StencilPassOp = StringToStencilOp(desc.stencilPassOpFront);
		psoDesc.DepthStencilState.FrontFace.StencilFunc = StringToComparisonFunc(desc.stencilFuncFront);

		psoDesc.DepthStencilState.BackFace.StencilFailOp = StringToStencilOp(desc.stencilFailOpBack);
		psoDesc.DepthStencilState.BackFace.StencilDepthFailOp = StringToStencilOp(desc.stencilDepthFailOpBack);
		psoDesc.DepthStencilState.BackFace.StencilPassOp = StringToStencilOp(desc.stencilPassOpBack);
		psoDesc.DepthStencilState.BackFace.StencilFunc = StringToComparisonFunc(desc.stencilFuncBack);

		psoDesc.SampleMask = desc.sampleMask;
		psoDesc.NumRenderTargets = desc.numRenderTargets;

		i = 0;
		for (const auto& format : desc.rtvFormats)
		{
			psoDesc.RTVFormats[i] = StringToDXGIFormat(format);
			i++;
		}
		psoDesc.DSVFormat = StringToDXGIFormat(desc.dsvFormat);
		psoDesc.SampleDesc.Count = desc.sampleCount;
		psoDesc.SampleDesc.Quality = desc.sampleQuality;



		// Try to find the PSO in the cache
		ID3D12PipelineState** ppPipelineState;
		PipelineState::PipelineStateDictionary()[desc.name] = std::make_unique<PipelineState>(ppPipelineState);
		if (FAILED(PipelineState::PSOLibrary->LoadGraphicsPipeline(
			AnsiToWString(desc.name).c_str(),
			&psoDesc, IID_PPV_ARGS(ppPipelineState))))
		{
			// Create new pipeline
			hr = pDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(ppPipelineState));
			if (FAILED(hr))
			{
				DPRINT_LOC("ERROR: COULD NOT CREATE GRAPHICS PIPELINE");
				std::exit(1);
			}

			// Store the pipeline in the library if creation succeeded
			PipelineState::PSOLibrary->StorePipeline(AnsiToWString(desc.name).c_str(), *ppPipelineState);
		}

		// One way or another, now we have a pipeline.

		psoIndex++;
	}

	static void SerializePipelines()
	{
		Microsoft::WRL::ComPtr<ID3DBlob> outBlob = nullptr;
		SIZE_T blobSize = PipelineState::PSOLibrary->GetSerializedSize();
		HRESULT hr = D3DCreateBlob(blobSize, outBlob.GetAddressOf());
		if (FAILED(hr))
		{
			DPRINT_LOC("ERROR: FAILED TO CREATE A BLOB");
			std::exit(1);
		}

		PipelineState::PSOLibrary->Serialize(outBlob->GetBufferPointer(), blobSize);
	}
};