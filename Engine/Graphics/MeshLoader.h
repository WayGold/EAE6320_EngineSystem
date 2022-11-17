#pragma once

#include <iostream>
#include <External/Lua/Includes.h>
#include <Engine/Graphics/Configuration.h>
#include <Engine/Asserts/Asserts.h>
#include <Engine/Platform/Platform.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Graphics/VertexFormats.h>

namespace eae6320 {
	namespace Graphics {
		cResult LoadAsset(const char* const i_path, lua_State*& o_luaState);
		cResult LoadTableValues(lua_State& io_luaState,
			Graphics::VertexFormats::sVertex_mesh*& vertexData, uint16_t*& indexData);
		cResult LoadTableValues_vertex(lua_State& io_luaState, Graphics::VertexFormats::sVertex_mesh*& vertexData);
		cResult LoadTableValues_index(lua_State& io_luaState, uint16_t*& indexData);
		cResult LoadTableValues_vertexCount(lua_State& io_luaState, uint16_t& vertexCount);
		cResult LoadTableValues_indexCount(lua_State& io_luaState, uint16_t& indexCount);
	}
}