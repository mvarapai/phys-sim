/*****************************************************************//**
 * \file   ResourceAssembler.cpp
 * \brief  Provides definition for resource assembler functions.
 * 
 * \author Mikalai Varapai
 * \date   May 2025
 *********************************************************************/

// Main design point of ResourceAssembler is to try to almost never modify it.

#include <fstream>
#include <iostream>

#include "RootSignature.h"
#include "Shader.h"
#include "PipelineState.h"

#include "ResourceAssembler.h"
#include "JSONLookup.h"

#include "nlohmann/json.hpp"

using namespace nlohmann;

void StaticResourceAssembler::AssembleStaticResources(
	ID3D12Device* pDevice,
	std::string resourceFilename)
{
	// First retrieve resource data into description vectors
	ReadJSON(resourceFilename);

	// Initialize root signatures
	DPRINT("Creating predefined root signatures...");
	RootSignature::Initialize(pDevice);
	DPRINT("Created %u root signatures.", RootSignature::RootSignatureDictionary().size());

	// Compile or read shaders

	DPRINT("Assembling shaders...");
	for (SHADER_DESC desc : shaderDescArray)
	{
		ShaderAssembler::LoadShader(desc);
	}
	DPRINT("Assembling shaders - SUCCESS");

	// Create PSOs

	for (const PSO_DESC& psoDesc : pipelineStateDescs)
	{
		PipelineStateAssembler::InitializePipelineState(psoDesc, pDevice);
	}
	PipelineStateAssembler::SerializePipelines();

	// Load textures
}

void StaticResourceAssembler::ReadJSON(const std::string& resourceFilename)
{
	std::string rcBasePath = "resources\\";

	// Read the JSON file with resource properties
	DPRINT("BEGIN READING \"%s\"...", resourceFilename);
	std::ifstream rcFileStream(rcBasePath + resourceFilename);
	json rcJSON;
	try
	{
		rcFileStream >> rcJSON;
	}
	catch (const json::parse_error& e)
	{
		DPRINT_LOC("ERROR: COULD NOT READ RESOURCE JSON FILE. ABORTING...");
		std::exit(1);
	}

	// At this point we must have the resource JSON loaded

	try
	{
		// READ TEXTURE DESCRIPTION
		DPRINT("Reading texture descriptions...");
		auto& textures = rcJSON["textures"];
		for (const auto& texture : textures)
		{
			TEXTURE2D_DESC tex;
			tex.name = texture["name"];
			tex.file = texture["file"];
			textureDescArray.push_back(tex);
		}
		DPRINT("Reading texture descriptions - SUCCESS.");

		// READ SHADER DESCRIPTION
		DPRINT("Reading shader descriptions...");
		auto& shaderJSON = rcJSON["shaders"];
		for (const auto& shaderDescJSON : shaderJSON)
		{
			SHADER_DESC desc;
			desc.name = shaderJSON["name"];
			desc.type = StringToShaderType(shaderJSON["type"]);
			desc.sourcecode = shaderJSON["source"];
			desc.root_signature = shaderJSON["root_signature"];
			desc.defines.reserve(shaderJSON["defines"].size());
			for (const auto& defineJSON : shaderJSON["defines"])
			{
				desc.defines.push_back({ shaderJSON["name"], std::string(shaderJSON["value"]) });
			}

			shaderDescArray.push_back(desc);
		}
		DPRINT("Reading shader descriptions - SUCCESS.");

		// READ PSO DESCRIPTIONS
		for (const auto& psoSourceFileJSON : rcJSON["pso"])
		{
			std::string psoSourceFile = "resouces\\pso\\";
			psoSourceFile += psoSourceFileJSON;
			psoSourceFile += ".json";

			// Open and read psoSourceFile
			std::ifstream psoInStream(psoSourceFile);
			json pso;
			psoInStream >> pso;

			DPRINT("Reading PSO at file \"%s\"...", psoSourceFile);

			// Retrieve data from the file
			PSO_DESC desc;

			desc.name = psoSourceFileJSON;

			desc.rootSignature = pso["RootSignature"];
			desc.VS = pso["Shaders"].value("VS", "null");
			desc.PS = pso["Shaders"].value("PS", "null");
			desc.HS = pso["Shaders"].value("HS", "null");
			desc.DS = pso["Shaders"].value("DS", "null");
			desc.GS = pso["Shaders"].value("GS", "null");

			desc.vertexType = pso.value("VertexType", "");	// There might not be a vertex type
			desc.primitiveTopologyType = pso["PrimitiveTopologyType"];

			desc.fillMode = pso["RasterizerState"]["FillMode"];
			desc.cullMode = pso["RasterizerState"]["CullMode"];
			desc.frontCounterClockwise = pso["RasterizerState"]["CounterClockwise"];
			desc.depthBias = pso["RasterizerState"]["DepthBias"];
			desc.depthBiasClamp = pso["RasterizerState"]["DepthBiasClamp"];
			desc.slopeScaledDepthBias = pso["RasterizerState"]["SlopeScaledDepthBias"];
			desc.depthClipEnable = pso["RasterizerState"]["DepthClipEnable"];
			desc.multisampleEnable = pso["RasterizerState"]["MultisampleEnable"];
			desc.antialiasedLineEnable = pso["RasterizerState"]["AntialiasedLineEnable"];
			desc.forcedSampleCount = pso["RasterizerState"]["ForcedSampleCount"];
			desc.conservativeRaster = pso["RasterizerState"]["ConservativeRaster"];

			desc.alphaToCoverageEnable = pso["BlendState"]["AlphaToCoverageEnable"];
			desc.independentBlendEnable = pso["BlendState"]["IndependentBlendEnable"];

			for (const auto& rtarget : pso["BlendState"]["RenderTarget"])
			{
				PSO_DESC::BLEND_STATE_RENDER_TARGET_DESC rtdesc;
				rtdesc.blendEnable = rtarget["BlendEnable"];
				rtdesc.logicOpEnable = rtarget["LogicOpEnable"];
				rtdesc.srcBlend = rtarget["SrcBlend"];
				rtdesc.destBlend = rtarget["DestBlend"];
				rtdesc.blendOp = rtarget["BlendOp"];
				rtdesc.srcBlendAlpha = rtarget["SrcBlendAlpha"];
				rtdesc.destBlendAlpha = rtarget["DestBlendAlpha"];
				rtdesc.blendOpAlpha = rtarget["BlendOpAlpha"];
				rtdesc.logicOp = rtarget["LogicOp"];
				rtdesc.renderTargetWriteMask = rtarget["RenderTargetWriteMask"];

				desc.blendStateRenderTargets.push_back(rtdesc);
			}

			desc.depthEnable = pso["DepthStencilState"]["DepthEnable"];
			desc.depthWriteMask = pso["DepthStencilState"]["DepthWriteMask"];
			desc.depthFunc = pso["DepthStencilState"]["DepthFunc"];
			desc.stencilEnable = pso["DepthStencilState"]["StencilEnable"];
			desc.stencilReadMask = pso["DepthStencilState"]["StencilReadMask"];
			desc.stencilWriteMask = pso["DepthStencilState"]["StencilWriteMask"];

			desc.stencilFailOpFront = pso["DepthStencilState"]["FrontFace"]["StencilFailOp"];
			desc.stencilDepthFailOpFront = pso["DepthStencilState"]["FrontFace"]["StencilDepthFailOp"];
			desc.stencilPassOpFront = pso["DepthStencilState"]["FrontFace"]["StencilPassOp"];
			desc.stencilFailOpFront = pso["DepthStencilState"]["FrontFace"]["StencilFunc"];

			desc.stencilFailOpBack = pso["DepthStencilState"]["BackFace"]["StencilFailOp"];
			desc.stencilDepthFailOpBack = pso["DepthStencilState"]["BackFace"]["StencilDepthFailOp"];
			desc.stencilPassOpBack = pso["DepthStencilState"]["BackFace"]["StencilPassOp"];
			desc.stencilFuncBack = pso["DepthStencilState"]["BackFace"]["StencilFunc"];

			desc.sampleMask = pso["SampleMask"];
			desc.numRenderTargets = pso["NumRenderTargets"];
			for (const auto& j : pso["RTVFormats"])
			{
				desc.rtvFormats.push_back(j);
			}
			desc.dsvFormat = pso["DSVFormat"];
			desc.sampleCount = pso["SampleDesc"]["SampleCount"];
			desc.sampleQuality = pso["SampleDesc"]["SampleQuality"];

			pipelineStateDescs.push_back(desc);

			DPRINT("Reading PSO at file \"%s\" - SUCCESS", psoSourceFile);
		}
	}
	catch (json::exception& e)
	{
		std::cerr << "JSON error: " << e.what() << "\n";
		std::exit(1);
	}
}
