/**********************************************************************
 * \file   DefaultDrawable.h
 * \brief  Defines a wrapper structure for geometry rendering
 * 
 * \author Mikalai Varapai
 * \date   October 2023
 *********************************************************************/

#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <string>
#include <vector>
#include <memory>

#include "PipelineState.h"
#include "GeometryBuffer.h"
#include "RootSignature.h"
#include "Texture2D.h"

// Goal: introduce general Draw() function without arguments
class IDrawable
{
public:

	static std::vector<std::unique_ptr<IDrawable>>& DrawableArray()
	{
		static std::vector<std::unique_ptr<IDrawable>> vector;
		return vector;
	}

protected:

	/* Pointers to data structures, in order of significance */
	RootSignature*	pRootSignature = nullptr;
	PipelineState*	pPipelineState = nullptr;
	UINT			renderPriority = UINT_MAX;

public:
	virtual void Draw() = 0;
};

/**
 * Implementation for IDrawable interface.
 * Uses default PSO. VB and IB are set manually
 */
class DefaultDrawable : public IDrawable
{
public:
	void Draw() override
	{

	}
};

/**
 * Static class used to create IDrawable's.
 */
class DrawableAssembler
{

};
