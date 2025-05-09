/*****************************************************************//**
 * \file   simulation.h
 * \brief  Simulation class. Sends data to DirectX for render.
 * 
 * \author Mikalai Varapai
 * \date   May 2025
 *********************************************************************/

#pragma once

#include "communication.h"

class Simulation
{
public:
	void Initialize();			// Define static geometry and starting conditions
	void SimulateFrame();		// Update dynamic geometry
};
