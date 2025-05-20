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
#include "PredefinedVertexStructures.h"

#include "Buffer.h"

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

template<typename VertexType, size_t BufferSize>
class GeometryBufferDynamic : public GeometryBufferBase
{
	std::unique_ptr<RingBuffer<VertexType, BufferSize>> VertexBuffer;

	static constexpr int INDEX_BUFFER_SIZE_MULTIPLIER = 6;
	std::unique_ptr<RingBuffer<uint32_t, INDEX_BUFFER_SIZE_MULTIPLIER * BufferSize>> IndexBuffer;

public:
	// Implicitly loads vertex data into GPU resource 
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView(UINT beginVertexLocation, UINT numVertices) override
	{
		D3D12_VERTEX_BUFFER_VIEW vbv{ };
		vbv.BufferLocation = VertexBuffer->GetGPUHandleSingle(beginVertexLocation);
		vbv.StrideInBytes = sizeof(VertexType);
		vbv.SizeInBytes = sizeof(VertexType) * numVertices;

		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView(UINT beginIndexLocation, UINT numIndices) override
	{
		D3D12_INDEX_BUFFER_VIEW ibv{ };
		ibv.BufferLocation = IndexBuffer->GetGPUHandleSingle(beginIndexLocation);
		ibv.Format = DXGI_FORMAT_R32_UINT;
		ibv.SizeInBytes = sizeof(uint32_t) * numIndices;
	
		return ibv;
	}

private:
	GeometryBufferDynamic(ID3D12Device* pDevice,
		std::vector<VertexType>& vertexData,
		std::vector<uint32_t>& indexData)
	{
		// Initialize the buffers

		VertexBuffer = std::make_unique<RingBuffer<
			VertexType, BufferSize>>(pDevice, false, vertexData);
		IndexBuffer = std::make_unique<RingBuffer<
			uint32_t, INDEX_BUFFER_SIZE_MULTIPLIER * BufferSize>>(pDevice, false, indexData);
	}

	void UpdateVertexBuffer(std::vector<VertexType>& vertexData)
	{
		VertexBuffer->SetCPUData(vertexData);
	}

	void UpdateIndexBuffer(std::vector<uint32_t>& indexData)
	{
		IndexBuffer->SetCPUData(indexData);
	}
};

// For future endeavors
template<typename VertexType>
class GeometryBufferStatic : public GeometryBufferBase
{
};
