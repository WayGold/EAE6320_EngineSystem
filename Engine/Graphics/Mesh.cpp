#include "Mesh.h"
#include "MeshLoader.h"
#include <fstream>
#include <chrono>

eae6320::cResult eae6320::Graphics::Mesh::Mesh::createMesh(Mesh*& i_ptr, const char* const i_path)
{
	cResult result = Results::Success;
	i_ptr = new Mesh();

	result = LoadBinary(i_path, i_ptr);

	if(!i_ptr)
		result = Results::Failure;

	return result;
}

eae6320::cResult eae6320::Graphics::Mesh::LoadBinary(const char* i_path, Mesh* o_mesh)
{
	auto result = Results::Success;

	// DEBUG - LOAD TIME FOR BINARY FORMAT
	auto tick_before = std::chrono::high_resolution_clock::now();

	std::ifstream infile(i_path, std::ifstream::binary);

	// get size of file
	infile.seekg(0, infile.end);
	long size = static_cast<long>(infile.tellg());
	infile.seekg(0);

	// allocate memory for file content
	char* buffer = new char[size];

	// read content of infile
	infile.read(buffer, size);

	auto currentOffset = reinterpret_cast<uintptr_t>(buffer);
	const auto finalOffset = currentOffset + size;

	uint16_t vertexCount = *reinterpret_cast<uint16_t*>(buffer);
	uint16_t indexCount = *reinterpret_cast<uint16_t*>(buffer + 2);
	size_t pointsSize = sizeof(Graphics::VertexFormats::sVertex_mesh) * vertexCount;

	Graphics::VertexFormats::sVertex_mesh* points = reinterpret_cast<Graphics::VertexFormats::sVertex_mesh*>(buffer + 4);
	uint16_t* triangles = reinterpret_cast<uint16_t*>(buffer + 4 + pointsSize);

	// FIXME: DEBUG HUMAN READABLE LOAD TIME
	auto tick_after = std::chrono::high_resolution_clock::now();
	auto second = std::chrono::duration_cast<std::chrono::microseconds>(tick_after - tick_before);

	Logging::OutputMessage("Time used to load binary file for:\n %s\n%d (ms)", i_path, second.count());

	o_mesh->init(points, triangles, vertexCount, indexCount);

	if (!o_mesh)
		result = Results::Failure;

	// De-allocate buffer
	delete[] buffer;
	infile.close();

	return result;
}