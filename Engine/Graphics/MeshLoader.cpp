#include "MeshLoader.h"

eae6320::cResult eae6320::Graphics::LoadAsset(const char* const i_path, lua_State*& o_luaState)
{
	auto result = eae6320::Results::Success;

	// Create a new Lua state
	lua_State* luaState = nullptr;

	//eae6320::cScopeGuard scopeGuard_onExit([&luaState]
	//	{
	//		if (luaState)
	//		{
	//			// If I haven't made any mistakes
	//			// there shouldn't be anything on the stack
	//			// regardless of any errors
	//			EAE6320_ASSERT(lua_gettop(luaState) == 0);

	//			lua_close(luaState);
	//			luaState = nullptr;
	//		}
	//	});

	{
		luaState = luaL_newstate();
		if (!luaState)
		{
			result = eae6320::Results::OutOfMemory;
			std::cerr << "Failed to create a new Lua state" << std::endl;
			return result;
		}
	}

	// Load the asset file as a "chunk",
	// meaning there will be a callable function at the top of the stack
	const auto stackTopBeforeLoad = lua_gettop(luaState);
	{
		const auto luaResult = luaL_loadfile(luaState, i_path);
		if (luaResult != LUA_OK)
		{
			result = eae6320::Results::Failure;
			std::cerr << lua_tostring(luaState, -1) << std::endl;
			// Pop the error message
			lua_pop(luaState, 1);
			return result;
		}
	}
	// Execute the "chunk", which should load the asset
	// into a table at the top of the stack
	{
		constexpr int argumentCount = 0;
		constexpr int returnValueCount = LUA_MULTRET;	// Return _everything_ that the file returns
		constexpr int noMessageHandler = 0;
		const auto luaResult = lua_pcall(luaState, argumentCount, returnValueCount, noMessageHandler);
		if (luaResult == LUA_OK)
		{
			// A well-behaved asset file will only return a single value
			const auto returnedValueCount = lua_gettop(luaState) - stackTopBeforeLoad;
			if (returnedValueCount == 1)
			{
				// A correct asset file _must_ return a table
				if (!lua_istable(luaState, -1))
				{
					result = eae6320::Results::InvalidFile;
					std::cerr << "Asset files must return a table (instead of a "
						<< luaL_typename(luaState, -1) << ")" << std::endl;
					// Pop the returned non-table value
					lua_pop(luaState, 1);
					return result;
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				std::cerr << "Asset files must return a single table (instead of "
					<< returnedValueCount << " values)" << std::endl;
				// Pop every value that was returned
				lua_pop(luaState, returnedValueCount);
				return result;
			}
		}
		else
		{
			result = eae6320::Results::InvalidFile;
			std::cerr << lua_tostring(luaState, -1);
			// Pop the error message
			lua_pop(luaState, 1);
			return result;
		}
	}

	// If this code is reached the asset file was loaded successfully,
	// and its table is now at index -1

	// A scope guard is used to pop the asset as soon as the scope (this function) is left
	/*eae6320::cScopeGuard scopeGuard_popAssetTable([luaState]
		{
			lua_pop(luaState, 1);
		});*/

		// Load the values, assuming that the asset table is at index -1
		/*result = LoadTableValues(*luaState, vertexCount, vertexData, indexCount, indexData);*/
	o_luaState = luaState;
	return result;
}

eae6320::cResult eae6320::Graphics::LoadTableValues(lua_State& io_luaState,
	Graphics::VertexFormats::sVertex_mesh*& vertexData, uint16_t*& indexData)
{
	auto result = eae6320::Results::Success;

	if (!(result = LoadTableValues_vertex(io_luaState, vertexData)))
	{
		return result;
	}
	if (!(result = LoadTableValues_index(io_luaState, indexData)))
	{
		return result;
	}

	return result;
}

eae6320::cResult eae6320::Graphics::LoadTableValues_vertex(lua_State& io_luaState,
	Graphics::VertexFormats::sVertex_mesh*& vertexData)
{
	auto result = eae6320::Results::Success;
	constexpr auto* const key = "vertex";

	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);

	eae6320::cScopeGuard scopeGuard_popTextures([&io_luaState]
		{
			lua_pop(&io_luaState, 1);
		});

	if (lua_istable(&io_luaState, -1))
	{
		std::cout << "Iterating through every vertex data:" << std::endl;
		const auto vertex_count = luaL_len(&io_luaState, -1);

		for (int i = 1; i <= vertex_count; ++i)
		{
			lua_pushinteger(&io_luaState, i);
			lua_gettable(&io_luaState, -2);

			eae6320::cScopeGuard scopeGuard_popTexturePath([&io_luaState]
				{
					lua_pop(&io_luaState, 1);
				});

			lua_pushinteger(&io_luaState, 1);
			lua_gettable(&io_luaState, -2);
			vertexData[i - 1].x = (float)lua_tonumber(&io_luaState, -1);
			lua_pop(&io_luaState, 1);

			lua_pushinteger(&io_luaState, 2);
			lua_gettable(&io_luaState, -2);
			vertexData[i - 1].y = (float)lua_tonumber(&io_luaState, -1);
			lua_pop(&io_luaState, 1);

			lua_pushinteger(&io_luaState, 3);
			lua_gettable(&io_luaState, -2);
			vertexData[i - 1].z = (float)lua_tonumber(&io_luaState, -1);
			lua_pop(&io_luaState, 1);
		}
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		std::cerr << "The value at \"" << key << "\" must be a table "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
		return result;
	}

	return result;
}

eae6320::cResult eae6320::Graphics::LoadTableValues_index(lua_State& io_luaState, uint16_t*& indexData)
{
	auto result = eae6320::Results::Success;
	constexpr auto* const key = "index";

	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);

	eae6320::cScopeGuard scopeGuard_popTextures([&io_luaState]
		{
			lua_pop(&io_luaState, 1);
		});

	if (lua_istable(&io_luaState, -1))
	{
		std::cout << "Iterating through every index data:" << std::endl;
		const auto count = luaL_len(&io_luaState, -1);

		for (int i = 1; i <= count; ++i)
		{
			lua_pushinteger(&io_luaState, i);
			lua_gettable(&io_luaState, -2);

			eae6320::cScopeGuard scopeGuard_popTexturePath([&io_luaState]
				{
					lua_pop(&io_luaState, 1);
				});

			indexData[i - 1] = (int)lua_tonumber(&io_luaState, -1);
		}
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		std::cerr << "The value at \"" << key << "\" must be a table "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
		return result;
	}

	return result;
}

eae6320::cResult eae6320::Graphics::LoadTableValues_vertexCount(lua_State& io_luaState, uint16_t& vertexCount)
{
	auto result = eae6320::Results::Success;
	constexpr auto* const key = "vertex";

	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);

	eae6320::cScopeGuard scopeGuard_popTextures([&io_luaState]
		{
			lua_pop(&io_luaState, 1);
		});

	if (lua_istable(&io_luaState, -1))
	{
		std::cout << "Getting Vertex Count:" << std::endl;
		const auto vertex_count = luaL_len(&io_luaState, -1);
		vertexCount = (uint16_t)vertex_count;
		if (vertexCount != vertex_count) {
			return Results::Failure;
		}
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		std::cerr << "The value at \"" << key << "\" must be a table "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
		return result;
	}

	return result;
}

eae6320::cResult eae6320::Graphics::LoadTableValues_indexCount(lua_State& io_luaState, uint16_t& indexCount)
{
	auto result = eae6320::Results::Success;
	constexpr auto* const key = "index";

	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);

	eae6320::cScopeGuard scopeGuard_popTextures([&io_luaState]
		{
			lua_pop(&io_luaState, 1);
		});

	if (lua_istable(&io_luaState, -1))
	{
		std::cout << "Getting Index Count:" << std::endl;
		const auto count = luaL_len(&io_luaState, -1);
		indexCount = (uint16_t)count;
		if (indexCount != count) {
			return Results::Failure;
		}
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		std::cerr << "The value at \"" << key << "\" must be a table "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
		return result;
	}

	return result;
}