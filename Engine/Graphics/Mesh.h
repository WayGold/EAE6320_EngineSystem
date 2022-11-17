#pragma once

/*
	This file declares the external interface for the Mesh system
*/

#include <vector>
#include <Engine/Results/Results.h>
#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <vector>
#include "VertexFormats.h"
#include "cVertexFormat.h"

#if defined( EAE6320_PLATFORM_WINDOWS )
#include <Engine/Windows/Includes.h>
#include <Engine/Assets/ReferenceCountedAssets.h>
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
		namespace Mesh {

			class Mesh
			{
			public:
				unsigned int m_vertexCount = 0;
				unsigned int m_indexCount = 0;

#ifdef EAE6320_PLATFORM_GL
				// A vertex buffer holds the data for each vertex
				GLuint m_vertexBufferId = 0;
				GLuint m_indexBufferId = 0;
				// A vertex array encapsulates the vertex data as well as the vertex input layout
				GLuint m_vertexArrayId = 0;
#endif


#ifdef EAE6320_PLATFORM_D3D
				eae6320::Graphics::cVertexFormat* m_vertexFormat = nullptr;

				// A vertex buffer holds the data for each vertex
				ID3D11Buffer* m_vertexBuffer = nullptr;
				ID3D11Buffer* m_indexBuffer = nullptr;
#endif
				void Draw();
				static cResult createMesh(Mesh*& i_ptr, const char* const i_path);
				void init(VertexFormats::sVertex_mesh points[], uint16_t triangles[],
					unsigned int i_vertexCount, unsigned int i_indexCount);

				EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS()
				EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(Mesh)
				EAE6320_ASSETS_DECLAREREFERENCECOUNT()

			private:
				Mesh() {};
				~Mesh();
			};

			cResult LoadBinary(const char* i_path, Mesh* o_mesh);
		} 
	}
}