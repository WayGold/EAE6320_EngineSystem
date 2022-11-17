/*
	This class is your specific game
*/

#ifndef EAE6320_CMyGame_H
#define EAE6320_CMyGame_H

#define M_PI				3.1415926
#define MAX_ACCELERATION	2
#define MAX_SPEED			4
#define SLOW_RADIUS			15
#define TARGET_RADIUS		0.01f
#define MAX_ROTATION		3.14f
#define MAX_ANG_ACCEL		3.14f / 4.0f
#define SLOW_RADIUS_ROT		3.14f / 4.0f
#define TARGET_RADIUS_ROT	3.14f / 4096.0f
#define WANDER_OFFSET		0.5
#define WANDER_RATE			3.14f * 4

// Includes
//=========

#include <Engine/Application/iApplication.h>
#include <Engine/Results/Results.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Graphics/Graphics.h>
#include <Engine/Physics/sRigidBodyState.h>
#include <Engine/Math/cMatrix_transformation.h>
#include <Engine/Graphics/RenderObject.h>
#include <Engine/AIService/AIService.h>

#if defined( EAE6320_PLATFORM_WINDOWS )
	#include "Resource Files/Resource.h"
#endif

// Class Declaration
//==================

namespace eae6320
{
	class cMyGame final : public Application::iApplication
	{
		// Inherited Implementation
		//=========================

	private:
		// Data
		int gameObjectCount;
		int effectCount;
		RenderObject::RenderObject* m_gameObject[10];
		Mesh::Mesh* meshArr[10];
		Effect::Effect* effectArr[10];
		Physics::sRigidBodyState m_gameObjectRigidBody[10];
		Physics::sRigidBodyState m_camera;

		// AI Service
		AIMovement::DynamicSeek* dynamicSeek = nullptr;
		AIMovement::DynamicArrive* dynamicArrive = nullptr;
		AIMovement::DynamicLookWhereYouAreGoing* dynamicLWYG = nullptr;
		AIMovement::DynamicWander* dynamicWander = nullptr;

		// Configuration
		//--------------

#if defined( EAE6320_PLATFORM_WINDOWS )
		// The main window's name will be displayed as its caption (the text that is displayed in the title bar).
		// You can make it anything that you want, but please keep the platform name and debug configuration at the end
		// so that it's easy to tell at a glance what kind of build is running.
		const char* GetMainWindowName() const final
		{
			return "Wei Zeng's EAE6320 Example Game"
				" -- "
#if defined( EAE6320_PLATFORM_D3D )
				"Direct3D"
#elif defined( EAE6320_PLATFORM_GL )
				"OpenGL"
#endif
#ifdef _DEBUG
				" -- Debug"
#endif
			;
		}
		// Window classes are almost always identified by name;
		// there is a unique "ATOM" associated with them,
		// but in practice Windows expects to use the class name as an identifier.
		// If you don't change the name below
		// your program could conceivably have problems if it were run at the same time on the same computer
		// as one of your classmate's.
		// You don't need to worry about this for our class,
		// but if you ever ship a real project using this code as a base you should set this to something unique
		// (a generated GUID would be fine since this string is never seen)
		const char* GetMainWindowClassName() const final { return "Wei Zeng's EAE6320 Example Main Window Class"; }
		// The following three icons are provided:
		//	* IDI_EAEGAMEPAD
		//	* IDI_EAEALIEN
		//	* IDI_VSDEFAULT_LARGE / IDI_VSDEFAULT_SMALL
		// If you want to try creating your own a convenient website that will help is: http://icoconvert.com/
		const WORD* GetLargeIconId() const final { static constexpr WORD iconId_large = IDI_EAEGAMEPAD; return &iconId_large; }
		const WORD* GetSmallIconId() const final { static constexpr WORD iconId_small = IDI_EAEGAMEPAD; return &iconId_small; }
#endif

		// Run
		//----

		void UpdateBasedOnInput() final;
		void UpdateSimulationBasedOnInput() final;
		void UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate) final;
		void SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime,
			const float i_elapsedSecondCount_sinceLastSimulationUpdate);

		// Initialize / Clean Up
		//----------------------
		cResult Initialize() final;
		cResult CleanUp() final;

		// Init Rigid Bodies
		cResult RigidBody_Init();

		// AI SERVICE INIT / CLEAN UP
		cResult AI_Init();
		cResult AI_Cleanup();
	};
}

// Result Definitions
//===================

namespace eae6320
{
	namespace Results
	{
		namespace Application
		{
			// You can add specific results for your game here:
			//	* The System should always be Application
			//	* The __LINE__ macro is used to make sure that every result has a unique ID.
			//		That means, however, that all results _must_ be defined in this single file
			//		or else you could have two different ones with equal IDs.
			//	* Note that you can define multiple Success codes.
			//		This can be used if the caller may want to know more about how a function succeeded.
			constexpr cResult ExampleResult( IsFailure, eSystem::Application, __LINE__, Severity::Default );
		}
	}
}

#endif	// EAE6320_CMyGame_H
