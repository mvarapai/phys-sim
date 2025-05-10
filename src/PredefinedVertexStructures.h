/*****************************************************************//**
 * \file   PredefinedVertexStructures.h
 * \brief  Defines vertex types used in application.
 * 
 * \author Mikalai Varapai
 * \date   May 2025
 *********************************************************************/

#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <array>

class VertexBase { };

// Position only
struct VertexPosOnly : VertexBase
{
private:
	constexpr static D3D12_INPUT_ELEMENT_DESC _arrVertexPosOnly[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

public:
	constexpr static D3D12_INPUT_LAYOUT_DESC InputLayoutDesc
	{
		_arrVertexPosOnly,
		static_cast<UINT>(std::size(_arrVertexPosOnly))
	};

public:
	DirectX::XMFLOAT3 _Pos;
};

// Position and color
struct VertexCol : VertexBase
{
private:
	static constexpr D3D12_INPUT_ELEMENT_DESC _arrVertexCol[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

public:
	static constexpr D3D12_INPUT_LAYOUT_DESC InputLayoutDesc
	{
		_arrVertexCol,
		static_cast<UINT>(std::size(_arrVertexCol))
	};

public:
	DirectX::XMFLOAT3 _Pos;
	DirectX::XMFLOAT4 _Color;
};

// Position and texture
struct VertexTex : VertexBase
{
private:
	static constexpr D3D12_INPUT_ELEMENT_DESC _arrVertexTex[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

public:
	static constexpr D3D12_INPUT_LAYOUT_DESC InputLayoutDesc
	{
		_arrVertexTex,
		static_cast<UINT>(std::size(_arrVertexTex))
	};

public:
	DirectX::XMFLOAT3 _Pos;
	DirectX::XMFLOAT2 _Tex;
};

// Position, normal, and texture
struct VertexPosNormTex : VertexBase
{
private:
	static constexpr D3D12_INPUT_ELEMENT_DESC _arrVertexPosNormTex[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

public:
	static constexpr D3D12_INPUT_LAYOUT_DESC InputLayoutDesc
	{
		_arrVertexPosNormTex,
		static_cast<UINT>(std::size(_arrVertexPosNormTex))
	};

public:
	DirectX::XMFLOAT3 _Pos;
	DirectX::XMFLOAT3 _Normal;
	DirectX::XMFLOAT2 _Tex;
};

// 2D position, UV and color
struct VertexUI : VertexBase
{
private:
	static constexpr D3D12_INPUT_ELEMENT_DESC _arrVertexUI[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 16,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

public:
	static constexpr D3D12_INPUT_LAYOUT_DESC InputLayoutDesc
	{
		_arrVertexUI,
		static_cast<UINT>(std::size(_arrVertexUI))
	};

public:
	DirectX::XMFLOAT2 _uiPos;
	DirectX::XMFLOAT2 _Tex;
	DirectX::XMFLOAT4 _Color;
};