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

#define MAX_ROOT_SIGNATURES 8

class StaticResourceManager;

// Factory static-only class to load static resources.
class StaticResourceAssembler
{
private:
	StaticResourceAssembler() = default;
public:

	static void AssembleStaticResources(StaticResourceManager& manager,
		ID3D12Device* pDevice,
		std::string resourceFilename);

// Subroutines for resource initialization.
private:
	void InitPipelineStates();
	void LoadTextures();
	void LoadMaterials();
};

// Class that contains and manages all D3D12 resources, including
// PSOs, shaders, and input layouts.
class StaticResourceManager
{
private:

	std::unordered_map<std::string,
		Microsoft::WRL::ComPtr<ID3D12PipelineState>>	mPipelineStates;

	std::unordered_map<std::string,
		std::unique_ptr<Shader>>						mShaders;

	std::vector<std::unique_ptr<RootSignature>>			mRootSignatures;

	friend class StaticResourceAssembler;
};
