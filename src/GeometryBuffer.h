/*****************************************************************//**
 * \file   GeometryBuffer.h
 * \brief  Defines static and dynamic geometry buffers.
 * 
 * \author Mikalai Varapai
 * \date   May 2025
 *********************************************************************/

#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <memory>
#include <unordered_map>

#include "UploadBuffer.h"

class GeometryBufferBase
{
public:
	virtual D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView(UINT beginVertexLocation, UINT numVertices) = 0;
	virtual D3D12_INDEX_BUFFER_VIEW GetIndexBufferView(UINT beginIndexLocation, UINT numIndices) = 0;

	static std::unordered_map<std::string, std::unique_ptr<GeometryBufferBase>>& GeometryBufferDictionary()
	{
		static std::unordered_map<std::string, std::unique_ptr<GeometryBufferBase>> map;
		return map;
	}
};

template<typename VertexType>
class GeometryBufferDynamic : public GeometryBufferBase
{
public:
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView(UINT beginVertexLocation, UINT numVertices) override
	{

	}

	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView(UINT beginIndexLocation, UINT numIndices) override
	{

	}

private:
	GeometryBufferDynamic(ID3D12Device* pDevice, UINT elementCount)
	{
		pResource = std::make_unique<UploadBuffer<VertexType>>(pDevice, elementCount, false);
	
	}

	std::unique_ptr<UploadBuffer<VertexType>> pResource = nullptr;
};

template<typename VertexType>
class GeometryBufferStatic : public GeometryBufferBase
{
public:

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> pResource = nullptr;

};
