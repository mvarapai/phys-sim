#pragma once

#include <d3d12.h>
#include <string>
#include "DebugPrint.h"

enum class SHADER_TYPE
{
	SHADER_TYPE_VS,
	SHADER_TYPE_PS,
	SHADER_TYPE_HS,
	SHADER_TYPE_DS,
	SHADER_TYPE_GS,
	SHADER_TYPE_UNKNOWN,
};

inline std::string ShaderTypeToString(SHADER_TYPE type)
{
	switch (type)
	{
	case SHADER_TYPE::SHADER_TYPE_VS:
		return "VS";
	case SHADER_TYPE::SHADER_TYPE_PS:
		return "PS";
	case SHADER_TYPE::SHADER_TYPE_HS:
		return "HS";
	case SHADER_TYPE::SHADER_TYPE_DS:
		return "DS";
	case SHADER_TYPE::SHADER_TYPE_GS:
		return "GS";
	default:
		return "unknown";
	}
}

inline SHADER_TYPE StringToShaderType(std::string str)
{
	if (str == "VS") return SHADER_TYPE::SHADER_TYPE_VS;
	else if (str == "PS") return SHADER_TYPE::SHADER_TYPE_PS;
	else if (str == "HS") return SHADER_TYPE::SHADER_TYPE_HS;
	else if (str == "DS") return SHADER_TYPE::SHADER_TYPE_DS;
	else if (str == "GS") return SHADER_TYPE::SHADER_TYPE_GS;
	else return SHADER_TYPE::SHADER_TYPE_UNKNOWN;
}

// Conversion functions
inline D3D12_PRIMITIVE_TOPOLOGY_TYPE StringToPrimitiveTopologyType(const std::string& str)
{
	if (str == "LINE") return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	else if (str == "PATCH") return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
	else if (str == "POINT") return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	else if (str == "TRIANGLE") return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	DPRINT("CONVERSION ERROR: UNKNOWN PRIMITIVE TOPOLOGY TYPE \"%s\"", str.c_str());
	return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
}

inline D3D12_FILL_MODE StringToFillMode(const std::string& str)
{
	if (str == "SOLID") return D3D12_FILL_MODE_SOLID;
	else if (str == "WIREFRAME") return D3D12_FILL_MODE_WIREFRAME;

	DPRINT("CONVERSION ERROR: UNKNOWN FILL MODE \"%s\"", str.c_str());
	return D3D12_FILL_MODE_WIREFRAME;
}

inline D3D12_CULL_MODE StringToCullMode(const std::string& str)
{
	if (str == "BACK") return D3D12_CULL_MODE_BACK;
	else if (str == "FRONT") return D3D12_CULL_MODE_FRONT;
	else if (str == "NONE") return D3D12_CULL_MODE_NONE;

	DPRINT("CONVERSION ERROR: UNKNOWN CULL MODE \"%s\"", str.c_str());
	return D3D12_CULL_MODE_FRONT;
}

inline D3D12_CONSERVATIVE_RASTERIZATION_MODE StringToRasterizationMode(const std::string& str)
{
	if (str == "ON") return D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON;
	else if (str == "OFF") return D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	DPRINT("CONVERSION ERROR: UNKNOWN RASTERIZATION MODE \"%s\"", str.c_str());
	return D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
}

inline D3D12_BLEND StringToBlend(const std::string& str)
{
	if (str == "ZERO") return D3D12_BLEND_ZERO;
	else if (str == "ONE") return D3D12_BLEND_ONE;
	else if (str == "SRC_COLOR") return D3D12_BLEND_SRC_COLOR;
	else if (str == "INV_SRC_COLOR") return D3D12_BLEND_INV_SRC_COLOR;
	else if (str == "SRC_ALPHA") return D3D12_BLEND_SRC_ALPHA;
	else if (str == "INV_SRC_ALPHA") return D3D12_BLEND_INV_SRC_ALPHA;
	else if (str == "DEST_ALPHA") return D3D12_BLEND_DEST_ALPHA;
	else if (str == "INV_DEST_ALPHA") return D3D12_BLEND_INV_DEST_ALPHA;
	else if (str == "DEST_COLOR") return D3D12_BLEND_DEST_COLOR;
	else if (str == "INV_DEST_COLOR") return D3D12_BLEND_INV_DEST_COLOR;
	else if (str == "SRC_ALPHA_SAT") return D3D12_BLEND_SRC_ALPHA_SAT;
	else if (str == "BLEND_FACTOR") return D3D12_BLEND_BLEND_FACTOR;
	else if (str == "INV_BLEND_FACTOR") return D3D12_BLEND_INV_BLEND_FACTOR;
	else if (str == "SRC1_COLOR") return D3D12_BLEND_SRC1_COLOR;
	else if (str == "INV_SRC1_COLOR") return D3D12_BLEND_INV_SRC1_COLOR;
	else if (str == "SRC1_ALPHA") return D3D12_BLEND_SRC1_ALPHA;
	else if (str == "INV_SRC1_ALPHA") return D3D12_BLEND_INV_SRC1_ALPHA;
	else if (str == "ALPHA_FACTOR") return D3D12_BLEND_ALPHA_FACTOR;
	else if (str == "INV_ALPHA_FACTOR") return D3D12_BLEND_INV_ALPHA_FACTOR;

	DPRINT("CONVERSION ERROR: UNKNOWN BLEND \"%s\"", str.c_str());
	return D3D12_BLEND_ZERO;
}

inline D3D12_BLEND_OP StringToBlendOp(const std::string& str)
{
	if		(str == "ADD") return D3D12_BLEND_OP_ADD;
	else if (str == "SUBTRACT") return D3D12_BLEND_OP_SUBTRACT;
	else if (str == "REV_SUBTRACT") return D3D12_BLEND_OP_REV_SUBTRACT;
	else if (str == "MIN") return D3D12_BLEND_OP_MIN;
	else if (str == "MAX") return D3D12_BLEND_OP_MAX;

	DPRINT("CONVERSION ERROR: UNKNOWN BLEND OP \"%s\"", str.c_str());
	return D3D12_BLEND_OP_ADD;
}

inline D3D12_LOGIC_OP StringToLogicOp(const std::string& str)
{
	if		(str == "CLEAR")            return D3D12_LOGIC_OP_CLEAR;
	else if (str == "SET")              return D3D12_LOGIC_OP_SET;
	else if (str == "COPY")             return D3D12_LOGIC_OP_COPY;
	else if (str == "COPY_INVERTED")    return D3D12_LOGIC_OP_COPY_INVERTED;
	else if (str == "NOOP")             return D3D12_LOGIC_OP_NOOP;
	else if (str == "INVERT")           return D3D12_LOGIC_OP_INVERT;
	else if (str == "AND")              return D3D12_LOGIC_OP_AND;
	else if (str == "NAND")             return D3D12_LOGIC_OP_NAND;
	else if (str == "OR")               return D3D12_LOGIC_OP_OR;
	else if (str == "NOR")              return D3D12_LOGIC_OP_NOR;
	else if (str == "XOR")              return D3D12_LOGIC_OP_XOR;
	else if (str == "EQUIV")            return D3D12_LOGIC_OP_EQUIV;
	else if (str == "AND_REVERSE")      return D3D12_LOGIC_OP_AND_REVERSE;
	else if (str == "AND_INVERTED")     return D3D12_LOGIC_OP_AND_INVERTED;
	else if (str == "OR_REVERSE")       return D3D12_LOGIC_OP_OR_REVERSE;
	else if (str == "OR_INVERTED")      return D3D12_LOGIC_OP_OR_INVERTED;

	DPRINT("CONVERSION ERROR: UNKNOWN LOGIC OP \"%s\"", str.c_str());
	return D3D12_LOGIC_OP_NOOP;
}

inline D3D12_DEPTH_WRITE_MASK StringToDepthWriteMask(const std::string& str)
{
	if (str == "ALL") return D3D12_DEPTH_WRITE_MASK_ALL;
	else if (str == "ZERO") return D3D12_DEPTH_WRITE_MASK_ZERO;

	DPRINT("CONVERSION ERROR: UNKNOWN DEPTH WRITE MASK \"%s\"", str.c_str());
	return D3D12_DEPTH_WRITE_MASK_ZERO;
}

inline D3D12_COMPARISON_FUNC StringToComparisonFunc(const std::string& str)
{
	if (str == "NEVER") return D3D12_COMPARISON_FUNC_NEVER;
	else if (str == "LESS") return D3D12_COMPARISON_FUNC_LESS;
	else if (str == "EQUAL") return D3D12_COMPARISON_FUNC_EQUAL;
	else if (str == "LESS_EQUAL") return D3D12_COMPARISON_FUNC_LESS_EQUAL;
	else if (str == "GREATER") return D3D12_COMPARISON_FUNC_GREATER;
	else if (str == "NOT_EQUAL") return D3D12_COMPARISON_FUNC_NOT_EQUAL;
	else if (str == "GREATER_EQUAL") return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
	else if (str == "ALWAYS") return D3D12_COMPARISON_FUNC_ALWAYS;

	DPRINT("CONVERSION ERROR: UNKNOWN COMPARISON FUNC \"%s\"", str.c_str());
	return D3D12_COMPARISON_FUNC_ALWAYS;
}

inline D3D12_STENCIL_OP StringToStencilOp(const std::string& str)
{
	if (str == "KEEP") return D3D12_STENCIL_OP_KEEP;
	else if (str == "ZERO") return D3D12_STENCIL_OP_ZERO;
	else if (str == "REPLACE") return D3D12_STENCIL_OP_REPLACE;
	else if (str == "INCR_SAT") return D3D12_STENCIL_OP_INCR_SAT;
	else if (str == "DECR_SAT") return D3D12_STENCIL_OP_DECR_SAT;
	else if (str == "INVERT") return D3D12_STENCIL_OP_INVERT;
	else if (str == "INCR") return D3D12_STENCIL_OP_INCR;
	else if (str == "DECR") return D3D12_STENCIL_OP_DECR;

	DPRINT("CONVERSION ERROR: UNKNOWN COMPARISON FUNC \"%S\"", str.c_str());
	return D3D12_STENCIL_OP_ZERO;
}

inline DXGI_FORMAT StringToDXGIFormat(const std::string& str)
{
	if (str == "UNKNOWN")                 return DXGI_FORMAT_UNKNOWN;
	else if (str == "R8G8B8A8_UNORM")          return DXGI_FORMAT_R8G8B8A8_UNORM;
	else if (str == "R8G8B8A8_UNORM_SRGB")     return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	else if (str == "R16G16B16A16_FLOAT")      return DXGI_FORMAT_R16G16B16A16_FLOAT;
	else if (str == "R11G11B10_FLOAT")         return DXGI_FORMAT_R11G11B10_FLOAT;
	else if (str == "R32_FLOAT")               return DXGI_FORMAT_R32_FLOAT;
	else if (str == "R32G32_FLOAT")            return DXGI_FORMAT_R32G32_FLOAT;
	else if (str == "R32G32B32_FLOAT")         return DXGI_FORMAT_R32G32B32_FLOAT;
	else if (str == "R32G32B32A32_FLOAT")      return DXGI_FORMAT_R32G32B32A32_FLOAT;
	else if (str == "R10G10B10A2_UNORM")       return DXGI_FORMAT_R10G10B10A2_UNORM;
	else if (str == "D24_UNORM_S8_UINT")       return DXGI_FORMAT_D24_UNORM_S8_UINT;
	else if (str == "D32_FLOAT")               return DXGI_FORMAT_D32_FLOAT;
	else if (str == "R16G16_FLOAT")            return DXGI_FORMAT_R16G16_FLOAT;
	else if (str == "R16_FLOAT")               return DXGI_FORMAT_R16_FLOAT;
	else if (str == "R8_UNORM")                return DXGI_FORMAT_R8_UNORM;
	else if (str == "R8G8_UNORM")              return DXGI_FORMAT_R8G8_UNORM;
	else if (str == "R16G16_UNORM")            return DXGI_FORMAT_R16G16_UNORM;
	else if (str == "R32_UINT")                return DXGI_FORMAT_R32_UINT;
	else if (str == "R32G32_UINT")             return DXGI_FORMAT_R32G32_UINT;
	else if (str == "R16_UINT")                return DXGI_FORMAT_R16_UINT;
	else if (str == "R8_UINT")                 return DXGI_FORMAT_R8_UINT;
	else if (str == "BC1_UNORM")               return DXGI_FORMAT_BC1_UNORM;
	else if (str == "BC1_UNORM_SRGB")          return DXGI_FORMAT_BC1_UNORM_SRGB;
	else if (str == "BC3_UNORM")               return DXGI_FORMAT_BC3_UNORM;
	else if (str == "BC3_UNORM_SRGB")          return DXGI_FORMAT_BC3_UNORM_SRGB;

	DPRINT("CONVERSION ERROR: UNKNOWN DXGI_FORMAT \"%s\"", str.c_str());
	return DXGI_FORMAT_UNKNOWN;
}