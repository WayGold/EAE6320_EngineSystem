#pragma once
/*
	This file declares the external interface for the View system
*/

#include <Engine/Results/Results.h>
#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>

#if defined( EAE6320_PLATFORM_WINDOWS )
#include <Engine/Windows/Includes.h>
#endif

#ifdef EAE6320_PLATFORM_D3D
#include "Direct3D/Includes.h"
#endif

// Interface
//==========

using namespace std;

namespace eae6320
{
	namespace Graphics
	{

		struct sInitializationParameters
		{
#if defined( EAE6320_PLATFORM_WINDOWS )
			HWND mainWindow = NULL;
#if defined( EAE6320_PLATFORM_D3D )
			uint16_t resolutionWidth = 0, resolutionHeight = 0;
#elif defined( EAE6320_PLATFORM_GL )
			HINSTANCE thisInstanceOfTheApplication = NULL;
#endif
#endif
		};

		namespace View {

			class View {
			public:
#ifdef EAE6320_PLATFORM_D3D
				// In Direct3D "views" are objects that allow a texture to be used a particular way:
				// A render target view allows a texture to have color rendered to it
				ID3D11RenderTargetView* m_renderTargetView = nullptr;
				// A depth/stencil view allows a texture to have depth rendered to it
				ID3D11DepthStencilView* m_depthStencilView = nullptr;
#endif
				View(const sInitializationParameters& i_initializationParameters);
				~View();

				void clearBuffer(float i_clearColor[]);
			};
		}
	}
}