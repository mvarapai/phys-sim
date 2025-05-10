/*****************************************************************//**
 * \file   ResourceAssembler.cpp
 * \brief  Provides definition for resource assembler functions.
 * 
 * \author Mikalai Varapai
 * \date   May 2025
 *********************************************************************/

// Main design point of ResourceAssembler is to try to almost never modify it.

#include <fstream>

#include "ResourceAssembler.h"
#include "RootSignature.h"
#include "nlohmann/json.hpp"

using namespace nlohmann;

struct Texture
{
	std::string name;
	std::string filename;
};

void StaticResourceAssembler::AssembleStaticResources(StaticResourceManager& manager,
	ID3D12Device* pDevice,
	std::string resourceFilename)
{
	std::string rcBasePath = "resources\\";

	// Initialize root signatures
	DPRINT("Creating predefined root signatures...");
	manager.mRootSignatures = RootSignature::createAll();
	DPRINT("Created %u root signatures.", manager.mRootSignatures.size());

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

	auto& textures = rcJSON["textures"];
	if (!textures.is_array())
	{
		DPRINT_LOC("ERROR: TEXTURES FIELD IN JSON IS SUPPOSED TO BE AN ARRAY. ABORTING...");
		std::exit(0);
	}

	std::vector<Texture> textureArray;
	for (const auto& texture : textures)
	{
		Texture tex = { };
		tex.name = texture["name"];
		tex.filename = texture["file"];

		textureArray.push_back(tex);
	}

	// TODO
}
