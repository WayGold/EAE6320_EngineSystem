/*
	This file declares the external interface for the graphics system
*/

#ifndef EAE6320_GRAPHICS_H
#define EAE6320_GRAPHICS_H

// Includes
//=========

#include "Configuration.h"
#include "Mesh.h"
#include "Effect.h"
#include "View.h"
#include "BufferManager.h"
#include "RenderObject.h"

#include <cstdint>
#include <vector>
#include <Engine/Results/Results.h>
#include <Engine/Math/cMatrix_transformation.h>

#if defined( EAE6320_PLATFORM_WINDOWS )
	#include <Engine/Windows/Includes.h>
#endif

// Interface
//==========

namespace eae6320
{
	namespace Graphics
	{
		// Submission
		//-----------

		// These functions should be called from the application (on the application loop thread)

		// As the class progresses you will add your own functions for submitting data,
		// but the following is an example (that gets called automatically)
		// of how the application submits the total elapsed times
		// for the frame currently being submitted
		void SubmitElapsedTime( const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_simulationTime );
		void SubmitClearColor(std::vector<float> clearColor);
		//void SubmitMeshEffectData(std::pair<Mesh::Mesh*, Effect::Effect*>* i_pairArr, unsigned int i_gameObjectCount);
		void SubmitMeshEffectData(Mesh::Mesh* i_mesh, Effect::Effect* i_effect);
		void SubmitRenderObjectData(RenderObject::RenderObject* i_data, const Math::cMatrix_transformation& i_drawCallData);

		// Camera Data Submission
		void SubmitWorldToCameraData(const Math::cMatrix_transformation& i_data);
		void SubmitCameraToProjectedData(const Math::cMatrix_transformation& i_data);
		void SubmitDrawCallData(const Math::cMatrix_transformation& i_data);
		// When the application is ready to submit data for a new frame
		// it should call this before submitting anything
		// (or, said another way, it is not safe to submit data for a new frame
		// until this function returns successfully)
		cResult WaitUntilDataForANewFrameCanBeSubmitted( const unsigned int i_timeToWait_inMilliseconds );
		// When the application has finished submitting data for a frame
		// it must call this function
		cResult SignalThatAllDataForAFrameHasBeenSubmitted();

		// Render
		//-------

		// This is called (automatically) from the main/render thread.
		// It will render a submitted frame as soon as it is ready
		// (i.e. as soon as SignalThatAllDataForAFrameHasBeenSubmitted() has been called)
		void RenderFrame();

		// Initialize / Clean Up
		//----------------------

		cResult Initialize( const sInitializationParameters& i_initializationParameters );
		cResult CleanUp();
	}
}

#endif	// EAE6320_GRAPHICS_H
