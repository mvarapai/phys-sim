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

// Position only

struct VertexPosOnly
{
	DirectX::XMFLOAT3 _Pos;
};

constexpr D3D12_INPUT_ELEMENT_DESC _arrVertexPosOnly[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
};

constexpr D3D12_INPUT_LAYOUT_DESC InputLayoutDesc_VertexPosOnly
{
	_arrVertexPosOnly,
	static_cast<UINT>(std::size(_arrVertexPosOnly))
};


// ---------------------------------------------------------


// Position and color

struct VertexCol
{
	DirectX::XMFLOAT3 _Pos;
	DirectX::XMFLOAT4 _Color;
};

constexpr D3D12_INPUT_ELEMENT_DESC _arrVertexCol[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 12,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
};

constexpr D3D12_INPUT_LAYOUT_DESC InputLayoutDesc_VertexCol
{
	_arrVertexCol,
	static_cast<UINT>(std::size(_arrVertexCol))
};


// ---------------------------------------------------------


// Position and texture

struct VertexTex
{
	DirectX::XMFLOAT3 _Pos;
	DirectX::XMFLOAT2 _Tex;
};

constexpr D3D12_INPUT_ELEMENT_DESC _arrVertexTex[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
};

constexpr D3D12_INPUT_LAYOUT_DESC InputLayoutDesc_VertexTex
{
	_arrVertexTex,
	static_cast<UINT>(std::size(_arrVertexTex))
};


// ---------------------------------------------------------


// Position, normal, and texture

struct VertexPosNormTex
{
	DirectX::XMFLOAT3 _Pos;
	DirectX::XMFLOAT3 _Normal;
	DirectX::XMFLOAT2 _Tex;
};

constexpr D3D12_INPUT_ELEMENT_DESC _arrVertexPosNormTex[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
};

constexpr D3D12_INPUT_LAYOUT_DESC InputLayout_VertexPosNormTex
{
	_arrVertexPosNormTex,
	static_cast<UINT>(std::size(_arrVertexPosNormTex))
};


// ---------------------------------------------------------


// 2D position, UV and color

struct VertexUI
{
	DirectX::XMFLOAT2 _uiPos;
	DirectX::XMFLOAT2 _Tex;
	DirectX::XMFLOAT4 _Color;
};

constexpr D3D12_INPUT_ELEMENT_DESC _arrVertexUI[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 16,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
};

constexpr D3D12_INPUT_LAYOUT_DESC InputLayout_VertexUI
{
	_arrVertexUI,
	static_cast<UINT>(std::size(_arrVertexUI))
};