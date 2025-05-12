/*****************************************************************//**
 * \file   ResourceAssembler.h
 * \brief  Assembles the resources from config - textures, shaders and PSOs
 * 
 * \author Mikalai Varapai
 * \date   May 2025
 *********************************************************************/

#pragma once

#include <unordered_map>
#include <d3d12.h>
#include <wrl.h>
#include <string>

#include "DebugPrint.h"
#include "Shader.h"
#include "RootSignature.h"
#include "Texture2D.h"

#define MAX_ROOT_SIGNATURES 8

class StaticResourceManager;

// Factory static-only class to load static resources.
class StaticResourceAssembler
{
private:
	StaticResourceAssembler() = default;

	static std::vector<TEXTURE2D_DESC> textureDescArray;
	static std::vector<SHADER_DESC> shaderDescArray;
	static std::vector<PSO_DESC> pipelineStateDescs;
public:

	static void AssembleStaticResources(
		ID3D12Device* pDevice,
		std::string resourceFilename);

// Subroutines for resource initialization.
private:
	static void ReadJSON(const std::string& resourceFilename);

	void InitPipelineStates();
	void LoadTextures();
	void LoadMaterials();
};