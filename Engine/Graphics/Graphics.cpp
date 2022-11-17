#include "Graphics.h"

#include "cConstantBuffer.h"
#include "ConstantBufferFormats.h"
#include "cRenderState.h"
#include "cShader.h"
//#include "sContext.h"
#include "VertexFormats.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Concurrency/cEvent.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Time/Time.h>
#include <Engine/UserOutput/UserOutput.h>
#include <new>
#include <utility>
#include <algorithm>

using namespace eae6320::Graphics;

namespace {

	eae6320::Graphics::View::View* view = nullptr;

	// Constant buffer object
	eae6320::Graphics::cConstantBuffer s_constantBuffer_frame(eae6320::Graphics::ConstantBufferTypes::Frame);
	eae6320::Graphics::cConstantBuffer s_constantbuffer_drawcall(eae6320::Graphics::ConstantBufferTypes::DrawCall);
	// Submission Data
	//----------------
	
	// In our class there will be two copies of the data required to render a frame:
	//	* One of them will be in the process of being populated by the data currently being submitted by the application loop thread
	//	* One of them will be fully populated and in the process of being rendered from in the render thread
	// (In other words, one is being produced while the other is being consumed)
	sDataRequiredToRenderAFrame s_dataRequiredToRenderAFrame[2];
	auto* s_dataBeingSubmittedByApplicationThread = &s_dataRequiredToRenderAFrame[0];
	auto* s_dataBeingRenderedByRenderThread = &s_dataRequiredToRenderAFrame[1];
	// The following two events work together to make sure that
	// the main/render thread and the application loop thread can work in parallel but stay in sync:
	// This event is signaled by the application loop thread when it has finished submitting render data for a frame
	// (the main/render thread waits for the signal)
	eae6320::Concurrency::cEvent s_whenAllDataHasBeenSubmittedFromApplicationThread;
	// This event is signaled by the main/render thread when it has swapped render data pointers.
	// This means that the renderer is now working with all the submitted data it needs to render the next frame,
	// and the application loop thread can start submitting data for the following frame
	// (the application loop thread waits for the signal)
	eae6320::Concurrency::cEvent s_whenDataForANewFrameCanBeSubmittedFromApplicationThread;
	unsigned int s_gameObjectCountBeforeFrameCleanup = 0;
}

// Helper Declarations
//====================

namespace
{
	eae6320::cResult InitializeGeometry();
	eae6320::cResult InitializeShadingData();
	eae6320::cResult InitializeViews(const eae6320::Graphics::sInitializationParameters& i_initializationParameters);
}

// Interface
//==========

// Submission
//-----------

void eae6320::Graphics::SubmitElapsedTime(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_simulationTime)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	auto& constantData_frame = s_dataBeingSubmittedByApplicationThread->constantData_frame;
	constantData_frame.g_elapsedSecondCount_systemTime = i_elapsedSecondCount_systemTime;
	constantData_frame.g_elapsedSecondCount_simulationTime = i_elapsedSecondCount_simulationTime;
}

void eae6320::Graphics::SubmitClearColor(std::vector<float> clearColor)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	s_dataBeingSubmittedByApplicationThread->bgColor[0] = clearColor[0];
	s_dataBeingSubmittedByApplicationThread->bgColor[1] = clearColor[1];
	s_dataBeingSubmittedByApplicationThread->bgColor[2] = clearColor[2];
	s_dataBeingSubmittedByApplicationThread->bgColor[3] = clearColor[3];
}

void eae6320::Graphics::SubmitDrawCallData(const Math::cMatrix_transformation& i_data)
{
	auto index = s_dataBeingSubmittedByApplicationThread->gameObjectCount;
	s_dataBeingSubmittedByApplicationThread->constantData_drawCall[index].g_transform_localToWorld = i_data;
}

//void eae6320::Graphics::SubmitMeshEffectData(std::pair<Mesh::Mesh*, Effect::Effect*>* i_pairArr, 
//	unsigned int i_gameObjectCount)
//{
//	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
//	s_dataBeingSubmittedByApplicationThread->pairArr = i_pairArr;
//	s_dataBeingSubmittedByApplicationThread->gameObjectCount = i_gameObjectCount;
//	
//	for (unsigned int i = 0; i < i_gameObjectCount; i++) {
//		s_dataBeingSubmittedByApplicationThread->pairArr[i].first->IncrementReferenceCount();
//		s_dataBeingSubmittedByApplicationThread->pairArr[i].second->IncrementReferenceCount();
//	}
//}

void eae6320::Graphics::SubmitMeshEffectData(Mesh::Mesh* i_mesh, Effect::Effect* i_effect) 
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread->gameObjectCount < 10);

	s_dataBeingSubmittedByApplicationThread->meshArr[s_dataBeingSubmittedByApplicationThread->gameObjectCount] = i_mesh;
	s_dataBeingSubmittedByApplicationThread->effectArr[s_dataBeingSubmittedByApplicationThread->gameObjectCount] = i_effect;
	s_dataBeingSubmittedByApplicationThread->gameObjectCount++;

	i_mesh->IncrementReferenceCount();
	i_effect->IncrementReferenceCount();
}

void eae6320::Graphics::SubmitRenderObjectData(RenderObject::RenderObject* i_data, 
	const Math::cMatrix_transformation& i_drawCallData)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread->gameObjectCount < 10);

	auto i_mesh = i_data->m_mesh;
	auto i_effect = i_data->m_effect;
	auto index = s_dataBeingSubmittedByApplicationThread->gameObjectCount;

	s_dataBeingSubmittedByApplicationThread->meshArr[index] = i_mesh;
	s_dataBeingSubmittedByApplicationThread->effectArr[index] = i_effect;
	s_dataBeingSubmittedByApplicationThread->constantData_drawCall[index].g_transform_localToWorld = i_drawCallData;

	s_dataBeingSubmittedByApplicationThread->gameObjectCount++;

	i_mesh->IncrementReferenceCount();
	i_effect->IncrementReferenceCount();
}

void eae6320::Graphics::SubmitWorldToCameraData(const Math::cMatrix_transformation& i_data)
{
	s_dataBeingSubmittedByApplicationThread->constantData_frame.g_transform_worldToCamera = i_data;
}

void eae6320::Graphics::SubmitCameraToProjectedData(const Math::cMatrix_transformation& i_data)
{
	s_dataBeingSubmittedByApplicationThread->constantData_frame.g_transform_cameraToProjected = i_data;
}

eae6320::cResult eae6320::Graphics::WaitUntilDataForANewFrameCanBeSubmitted(const unsigned int i_timeToWait_inMilliseconds)
{
	return Concurrency::WaitForEvent(s_whenDataForANewFrameCanBeSubmittedFromApplicationThread, i_timeToWait_inMilliseconds);
}

eae6320::cResult eae6320::Graphics::SignalThatAllDataForAFrameHasBeenSubmitted()
{
	return s_whenAllDataHasBeenSubmittedFromApplicationThread.Signal();
}


// Render
//-------

void eae6320::Graphics::RenderFrame()
{
	// Wait for the application loop to submit data to be rendered
	{
		if (Concurrency::WaitForEvent(s_whenAllDataHasBeenSubmittedFromApplicationThread))
		{
			// Switch the render data pointers so that
			// the data that the application just submitted becomes the data that will now be rendered
			std::swap(s_dataBeingSubmittedByApplicationThread, s_dataBeingRenderedByRenderThread);
			// Once the pointers have been swapped the application loop can submit new data
			if (!s_whenDataForANewFrameCanBeSubmittedFromApplicationThread.Signal())
			{
				EAE6320_ASSERTF(false, "Couldn't signal that new graphics data can be submitted");
				Logging::OutputError("Failed to signal that new render data can be submitted");
				UserOutput::Print("The renderer failed to signal to the application that new graphics data can be submitted."
					" The application is probably in a bad state and should be exited");
				return;
			}
		}
		else
		{
			EAE6320_ASSERTF(false, "Waiting for the graphics data to be submitted failed");
			Logging::OutputError("Waiting for the application loop to submit data to be rendered failed");
			UserOutput::Print("The renderer failed to wait for the application to submit data to be rendered."
				" The application is probably in a bad state and should be exited");
			return;
		}
	}

	// Clearing image buffer
	view->clearBuffer(s_dataBeingRenderedByRenderThread->bgColor);

	BufferManager::updateConstantBuffer(s_dataBeingRenderedByRenderThread, s_constantBuffer_frame);

	// LOOP THROU THE ARRAY TO BIND EFFECT AND DRAW MESH
	for (unsigned int i = 0; i < s_dataBeingRenderedByRenderThread->gameObjectCount; i++) {
		// Update Drawcall Buffer
		auto& constantData_drawCall = s_dataBeingRenderedByRenderThread->constantData_drawCall[i];
		s_constantbuffer_drawcall.Update(&constantData_drawCall);
		// Bind Shader and Draw
		s_dataBeingRenderedByRenderThread->effectArr[i]->Bind();
		s_dataBeingRenderedByRenderThread->meshArr[i]->Draw();
	}
	

	// Everything has been drawn to the "back buffer", which is just an image in memory.
	// In order to display it the contents of the back buffer must be "presented"
	// (or "swapped" with the "front buffer", which is the image that is actually being displayed)
	BufferManager::presentBackBuffer();

	// After all of the data that was submitted for this frame has been used
	// you must make sure that it is all cleaned up and cleared out
	// so that the struct can be re-used (i.e. so that data for a new frame can be submitted to it)
	{
		// (At this point in the class there isn't anything that needs to be cleaned up)
		//dataRequiredToRenderFrame

		// LOOP THROUGH THE PAIR ARRAY TO DECREMENT MESH AND EFFECT AND RESET COUNT
		for (unsigned int i = 0; i < s_dataBeingRenderedByRenderThread->gameObjectCount; i++) {
			s_dataBeingRenderedByRenderThread->meshArr[i]->DecrementReferenceCount();
			s_dataBeingRenderedByRenderThread->effectArr[i]->DecrementReferenceCount();
		}

		s_gameObjectCountBeforeFrameCleanup = s_dataBeingRenderedByRenderThread->gameObjectCount;
		s_dataBeingRenderedByRenderThread->gameObjectCount = 0;
	}
}

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::Graphics::Initialize(const sInitializationParameters& i_initializationParameters)
{
	auto result = Results::Success;

	// Initialize the platform-specific context
	if (!(result = sContext::g_context.Initialize(i_initializationParameters)))
	{
		EAE6320_ASSERTF(false, "Can't initialize Graphics without context");
		return result;
	}

	// Initialize the platform-independent graphics objects
	{
		if (result = s_constantBuffer_frame.Initialize())
		{
			// There is only a single frame constant buffer that is reused
			// and so it can be bound at initialization time and never unbound
			s_constantBuffer_frame.Bind(
				// In our class both vertex and fragment shaders use per-frame constant data
				static_cast<uint_fast8_t>(eShaderType::Vertex) | static_cast<uint_fast8_t>(eShaderType::Fragment));
		}
		else
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without frame constant buffer");
			return result;
		}
	}

	// Initialize Drawcall constant buffer
	{
		if (result = s_constantbuffer_drawcall.Initialize())
		{
			// There is only a single frame constant buffer that is reused
			// and so it can be bound at initialization time and never unbound
			s_constantbuffer_drawcall.Bind(
				// In our class both vertex and fragment shaders use per-frame constant data
				static_cast<uint_fast8_t>(eShaderType::Vertex) | static_cast<uint_fast8_t>(eShaderType::Fragment));
		}
		else
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without draw call constant buffer");
			return result;
		}
	}

	// Initialize the events
	{
		if (!(result = s_whenAllDataHasBeenSubmittedFromApplicationThread.Initialize(Concurrency::EventType::ResetAutomaticallyAfterBeingSignaled)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without event for when data has been submitted from the application thread");
			return result;
		}
		if (!(result = s_whenDataForANewFrameCanBeSubmittedFromApplicationThread.Initialize(Concurrency::EventType::ResetAutomaticallyAfterBeingSignaled,
			Concurrency::EventState::Signaled)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without event for when data can be submitted from the application thread");
			return result;
		}
	}
	// Initialize the views
	{
		if (!(result = InitializeViews(i_initializationParameters)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without the views");
			return result;
		}
	}

	Logging::OutputMessage("Size of app thread budget - %d", sizeof(*s_dataBeingSubmittedByApplicationThread));
	Logging::OutputMessage("Size of render thread budget - %d", sizeof(*s_dataBeingRenderedByRenderThread));

	return result;
}

eae6320::cResult eae6320::Graphics::CleanUp()
{
	auto result = Results::Success;

	if (view)
	{
		delete view;
	}

	// LOOP THROUGH THE PAIR ARRAY TO DECREMENT MESH AND EFFECT AND RESET COUNT
	/*for (unsigned int i = 0; i < s_dataBeingRenderedByRenderThread->gameObjectCount; i++) {
		s_dataBeingSubmittedByApplicationThread->meshArr[i]->DecrementReferenceCount();
		s_dataBeingSubmittedByApplicationThread->effectArr[i]->DecrementReferenceCount();
	}
	delete s_dataBeingRenderedByRenderThread->pairArr;
	s_dataBeingRenderedByRenderThread->pairArr = nullptr;
	s_dataBeingRenderedByRenderThread->gameObjectCount = 0;*/

	// LOOP THROUGH THE PAIR ARRAY TO DECREMENT MESH AND EFFECT AND RESET COUNT
	for (unsigned int i = 0; i < s_gameObjectCountBeforeFrameCleanup; i++) {
		s_dataBeingSubmittedByApplicationThread->meshArr[i]->DecrementReferenceCount();
		s_dataBeingSubmittedByApplicationThread->effectArr[i]->DecrementReferenceCount();
	}
	s_dataBeingRenderedByRenderThread->gameObjectCount = 0;
	s_gameObjectCountBeforeFrameCleanup = 0;

	{
		const auto result_constantBuffer_frame = s_constantBuffer_frame.CleanUp();
		if (!result_constantBuffer_frame)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = result_constantBuffer_frame;
			}
		}
	}

	{
		const auto result_constantBuffer_drawcall = s_constantbuffer_drawcall.CleanUp();
		if (!result_constantBuffer_drawcall)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = result_constantBuffer_drawcall;
			}
		}
	}

	{
		const auto result_context = sContext::g_context.CleanUp();
		if (!result_context)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = result_context;
			}
		}
	}

	return result;
}


namespace {

	eae6320::cResult InitializeViews(const eae6320::Graphics::sInitializationParameters& i_initializationParameters)
	{
		view = new eae6320::Graphics::View::View(i_initializationParameters);
		return eae6320::Results::Success;
	}
}