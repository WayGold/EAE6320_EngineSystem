#pragma once

#ifndef EAE6320_MESHBUILDER_H
#define EAE6320_MESHBUILDER_H

#include <iostream>
#include <Engine/Graphics/Mesh.h>
#include <Engine/Graphics/Configuration.h>
#include <Engine/Platform/Platform.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Graphics/VertexFormats.h>
#include <Engine/Time/Time.h>
#include <Engine/Logging/Logging.h>
#include <Tools/AssetBuildLibrary/iBuilder.h>
#include <Tools/AssetBuildLibrary/Functions.h>
#include <External/Lua/Includes.h>

namespace eae6320
{
	namespace Assets
	{
		class MeshBuilder final : public iBuilder
		{
		public:

			// Inherited Implementation
			//=========================

		private:

			// Build
			//------
			cResult Build(const std::vector<std::string>& i_arguments) final;
			cResult Load(const char* const i_path, const char* const o_path);
			cResult LoadAsset(const char* const i_path, lua_State*& o_luaState);
			cResult LoadTableValues(lua_State& io_luaState,
				Graphics::VertexFormats::sVertex_mesh*& vertexData, uint16_t*& indexData);
			cResult LoadTableValues_vertex(lua_State& io_luaState, Graphics::VertexFormats::sVertex_mesh*& vertexData);
			cResult LoadTableValues_index(lua_State& io_luaState, uint16_t*& indexData);
			cResult LoadTableValues_vertexCount(lua_State& io_luaState, uint16_t& vertexCount);
			cResult LoadTableValues_indexCount(lua_State& io_luaState, uint16_t& indexCount);
		};
	}
}

#endif	