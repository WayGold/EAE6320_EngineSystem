#pragma once

/*
	This file declares the external interface for the Effect(shader) system
*/

#include "cShader.h"
#include "cRenderState.h"

#include <string>
#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/ScopeGuard/cScopeGuard.h>

#if defined( EAE6320_PLATFORM_WINDOWS )
#include <Engine/Windows/Includes.h>
#include <Engine/Assets/ReferenceCountedAssets.h>
#endif

#ifdef EAE6320_PLATFORM_D3D
#include "Direct3D/Includes.h"
#endif

using namespace std;

namespace eae6320
{
	namespace Graphics
	{
		namespace Effect {

			class Effect {
			public:

#ifdef EAE6320_PLATFORM_GL
				GLuint m_programId = 0;
#endif
				eae6320::Graphics::cShader* m_vertexShader = nullptr;
				eae6320::Graphics::cShader* m_fragmentShader = nullptr;

				EAE6320_ASSETS_DECLAREREFERENCECOUNT()

				eae6320::Graphics::cRenderState m_renderState;

				void Bind();
				static cResult createEffect(Effect*& o_ptr, string vertShader, string fragShader);

				EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS()
				EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(Effect)
				
			private:
				Effect() {};
				~Effect();

				void init(string vertShader, string fragShader);
			};
		}
	}
}