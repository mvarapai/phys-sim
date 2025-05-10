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

class StaticResourceManager;

class StaticResourceAssembler
{
	std::unordered_map<std::string,
		Microsoft::WRL::ComPtr<ID3D12PipelineState>>	mPipelineStates;

	void InitPipelineStates()
	{

	}

	void LoadTextures()
	{

	}

	void LoadMaterials()
	{

	}
};

// Class that contains and manages all D3D12 resources, including
// PSOs, shaders, and input layouts.
class StaticResourceManager
{

};
