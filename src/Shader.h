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
#include <d3dcompiler.h>
#include <wrl.h>
#include <type_traits>
#include <unordered_map>

#include "PredefinedVertexStructures.h"
#include "d3dUtil.h"
#include "JSONLookup.h"

// The intended usage is as follows:
//	1. Resource manager class stores instances of class Shader.
//	2. Then, these instances are initialized with ShaderBase<VertexType> by
//		the resource initializer. In constructor, VertexBase exposes pointers
//		to underlying bytecode, which are to be initialized by the caller,
//		although this class maintains its ownership.

struct SHADER_DESC
{
	struct SHADER_DEFINE
	{
		std::string name;
		std::string value;
	};

	std::string					name;
	SHADER_TYPE					type;
	std::string					sourcecode;
	std::string					root_signature;
	std::string					vertex_type;
	std::vector<SHADER_DEFINE>	defines;
};

class Shader
{
public:
	static std::unordered_map<std::string, std::unique_ptr<Shader>>& ShaderDictionary()
	{
		static std::unordered_map<std::string, std::unique_ptr<Shader>> map;
		return map;
	}

private:
	Microsoft::WRL::ComPtr<ID3DBlob>					bytecode = nullptr;

	Shader(ID3DBlob** &shaderBytecode)
	{
		shaderBytecode = bytecode.GetAddressOf();
	}

public:
	D3D12_SHADER_BYTECODE GetBytecode()
	{
		return { reinterpret_cast<BYTE*>(bytecode->GetBufferPointer()),
			bytecode->GetBufferSize() };
	}

	static D3D12_SHADER_BYTECODE GetBytecode(const std::string& name)
	{
		if (!ShaderDictionary().contains(name)) return { nullptr, 0 };

		Shader* pShader = ShaderDictionary()[name].get();
		return pShader->GetBytecode();
	}

	friend class ShaderAssembler;
};


class ShaderAssembler
{
public:
	static void LoadShader(SHADER_DESC desc)
	{
		// Put new unique_ptr into the dictionary and initialize the shader
		ID3DBlob** ppBlob;
		Shader::ShaderDictionary()[desc.name] = std::make_unique<Shader>(ppBlob);

		// Load the bytecode into the blob
		
		// 1. Try to find compiled shader code

		// Define path to shader bytecode, e.g.
		// csoPath = <EXE_DIR>\resources\shaders\<NAME>.cso
		HRESULT hr = S_OK;
		std::wstring csoPath = L"resources\\shaders\\";
		csoPath += AnsiToWString(desc.name);
		csoPath += L".cso";

		// Just read the file to blob
		hr = D3DReadFileToBlob(csoPath.c_str(), ppBlob);

		// If succeeded, exit the function
		if (SUCCEEDED(hr)) return;

		// 2. Compile from source code

		DPRINT("Could not load \"%s\" from bytecode. Falling back to compiling from file...", csoPath);

		// Define path to source
		std::wstring srcPath = L"resources\\shaders\\src\\";
		srcPath += AnsiToWString(desc.sourcecode);
		
		// Translate macros
		std::vector<D3D_SHADER_MACRO> macros;
		macros.reserve(desc.defines.size() + 1);
		for (SHADER_DESC::SHADER_DEFINE def : desc.defines)
		{
			macros.push_back({ def.name.c_str(), def.value.c_str() });
		}
		macros.push_back({ nullptr, nullptr });	// End of macro array

		// Get shader target
		std::string target = ToLowercaseCopy(ShaderTypeToString(desc.type));	// e.g. "vs"
		target += "_5_0";

		// Compile shader from source file
		DPRINT("Compiling shader \"%s\"...", srcPath);
		CompileShader(srcPath, macros.data(), ShaderTypeToString(desc.type), target, ppBlob);

		hr = S_OK;
		hr = D3DWriteBlobToFile((*ppBlob), csoPath.c_str(), FALSE);

		if (FAILED(hr)) DPRINT("Could not write the file.");
	}
};
