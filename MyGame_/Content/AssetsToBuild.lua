--[[
	This file lists every asset that must be built by the AssetBuildSystem
]]

return
{
	shaders =
	{
		{ path = "Shaders/Vertex/standard.shader", arguments = { "vertex" } },
		{ path = "Shaders/Fragment/standard.shader", arguments = { "fragment" } },
		{ path = "Shaders/Fragment/anim_color.shader", arguments = { "fragment" } },
		{ path = "Shaders/Fragment/anim_color1.shader", arguments = { "fragment" } },
		{ path = "Shaders/Vertex/vertexInputLayout_mesh.shader", arguments = { "vertex" } },
	},
	meshes = {
		{ path = "Meshes/Mesh1.lua" },
		{ path = "Meshes/Mesh2.lua" },
		{ path = "Meshes/Plane.lua" },
		{ path = "Meshes/Sphere.lua" },
		{ path = "Meshes/tube.lua" },
		{ path = "Meshes/helix.lua" },
		{ path = "Meshes/Boid.lua" },
		{ path = "Meshes/donut.lua" },
	}
}
