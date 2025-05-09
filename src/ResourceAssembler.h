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

class StaticResourceAssembler
{
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> 
};
