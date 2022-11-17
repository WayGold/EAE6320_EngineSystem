#include "../Mesh.h"
#include "../sContext.h"

void eae6320::Graphics::Mesh::Mesh::init(VertexFormats::sVertex_mesh points[], uint16_t triangles[],
									unsigned int i_vertexCount, unsigned int i_indexCount) {
	auto result = eae6320::Results::Success;

	m_vertexCount = i_vertexCount;
	m_indexCount = i_indexCount;
	
	for (unsigned int i = 0; i < i_indexCount;) {
		if (i % 3 == 0) {
			i++;
			continue;
		}
		if (i % 3 == 1) {
			auto temp = triangles[i];
			triangles[i] = triangles[i + 1];
			triangles[i + 1] = temp;
			i += 2;
			continue;
		}
	}

	auto* const direct3dDevice = eae6320::Graphics::sContext::g_context.direct3dDevice;
	EAE6320_ASSERT(direct3dDevice);

	// Vertex Format
	{
		if (!(result = eae6320::Graphics::cVertexFormat::Load(eae6320::Graphics::eVertexType::Mesh, m_vertexFormat,
			"data/Shaders/Vertex/vertexInputLayout_mesh.shader")))
		{
			EAE6320_ASSERTF(false, "Can't initialize geometry without vertex format");
			return;
		}
	}
	// Vertex Buffer
	{
		auto bufferSize = sizeof(points[0]) * i_vertexCount;
		EAE6320_ASSERT(bufferSize <= std::numeric_limits<decltype(D3D11_BUFFER_DESC::ByteWidth)>::max());

		auto bufferDescription = [bufferSize]
		{
			D3D11_BUFFER_DESC bufferDescription{};

			bufferDescription.ByteWidth = static_cast<unsigned int>(bufferSize);
			bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;	// In our class the buffer will never change after it's been created
			bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDescription.CPUAccessFlags = 0;	// No CPU access is necessary
			bufferDescription.MiscFlags = 0;
			bufferDescription.StructureByteStride = 0;	// Not used

			return bufferDescription;
		}();

		const auto initialData = [points]
		{
			D3D11_SUBRESOURCE_DATA initialData{};

			initialData.pSysMem = points;
			// (The other data members are ignored for non-texture buffers)

			return initialData;
		}();

		const auto result_create = direct3dDevice->CreateBuffer(&bufferDescription, &initialData, &m_vertexBuffer);
		if (FAILED(result_create))
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, "3D object vertex buffer creation failed (HRESULT %#010x)", result_create);
			eae6320::Logging::OutputError("Direct3D failed to create a 3D object vertex buffer (HRESULT %#010x)", result_create);
			return;
		}
	}

	// Index Buffer
	{
		auto idxBufferSize = sizeof(triangles[0]) * i_indexCount;
		auto bufferDescription = [idxBufferSize]

		{
			D3D11_BUFFER_DESC bufferDescription{};

			bufferDescription.ByteWidth = static_cast<unsigned int>(idxBufferSize);
			bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;	// In our class the buffer will never change after it's been created
			bufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bufferDescription.CPUAccessFlags = 0;	// No CPU access is necessary
			bufferDescription.MiscFlags = 0;
			bufferDescription.StructureByteStride = 0;	// Not used

			return bufferDescription;
		}();

		const auto initialData = [triangles]
		{
			D3D11_SUBRESOURCE_DATA initialData{};

			initialData.pSysMem = triangles;
			// (The other data members are ignored for non-texture buffers)

			return initialData;
		}(); 

		const auto result_create = direct3dDevice->CreateBuffer(&bufferDescription, &initialData, &m_indexBuffer);
		if (FAILED(result_create))
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, "3D object vertex buffer creation failed (HRESULT %#010x)", result_create);
			eae6320::Logging::OutputError("Direct3D failed to create a 3D object index buffer (HRESULT %#010x)", result_create);
			return;
		}
	}
}

eae6320::Graphics::Mesh::Mesh::~Mesh()
{
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}
	if (m_indexBuffer) {
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}
	if (m_vertexFormat)
	{
		m_vertexFormat->DecrementReferenceCount();
		m_vertexFormat = nullptr;
	}
}


void eae6320::Graphics::Mesh::Mesh::Draw()
{
	// Draw the geometry
	{
		auto* const direct3dImmediateContext = sContext::g_context.direct3dImmediateContext;
		EAE6320_ASSERT(direct3dImmediateContext);

		// Bind a specific vertex buffer to the device as a data source
		{
			EAE6320_ASSERT(m_vertexBuffer != nullptr);
			constexpr unsigned int startingSlot = 0;
			constexpr unsigned int vertexBufferCount = 1;
			// The "stride" defines how large a single vertex is in the stream of data
			constexpr unsigned int bufferStride = sizeof(VertexFormats::sVertex_mesh);
			// It's possible to start streaming data in the middle of a vertex buffer
			constexpr unsigned int bufferOffset = 0;
			direct3dImmediateContext->IASetVertexBuffers(startingSlot, vertexBufferCount, &m_vertexBuffer, &bufferStride, &bufferOffset);
		}
		// Specify what kind of data the vertex buffer holds
		{
			// Bind the vertex format (which defines how to interpret a single vertex)
			{
				EAE6320_ASSERT(m_vertexFormat != nullptr);
				m_vertexFormat->Bind();
			}
			// Set the topology (which defines how to interpret multiple vertices as a single "primitive";
			// the vertex buffer was defined as a triangle list
			// (meaning that every primitive is a triangle and will be defined by three vertices)
			direct3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}

		{
			EAE6320_ASSERT(m_indexBuffer);
			constexpr DXGI_FORMAT indexFormat = DXGI_FORMAT_R16_UINT;
			// The indices start at the beginning of the buffer
			constexpr unsigned int offset = 0;
			direct3dImmediateContext->IASetIndexBuffer(m_indexBuffer, indexFormat, offset);
		}

		// Render triangles from the currently-bound vertex buffer
		//{
		//	// As of this comment only a single triangle is drawn
		//	// (you will have to update this code in future assignments!)
		//	constexpr unsigned int triangleCount = 2;
		//	constexpr unsigned int vertexCountPerTriangle = 3;
		//	constexpr auto vertexCountToRender = triangleCount * vertexCountPerTriangle;
		//	// It's possible to start rendering primitives in the middle of the stream
		//	constexpr unsigned int indexOfFirstVertexToRender = 0;
		//	direct3dImmediateContext->Draw(vertexCountToRender, indexOfFirstVertexToRender);
		//}

		{
			// It's possible to start rendering primitives in the middle of the stream
			constexpr unsigned int indexOfFirstIndexToUse = 0;
			constexpr unsigned int offsetToAddToEachIndex = 0;
			direct3dImmediateContext->DrawIndexed(m_indexCount, indexOfFirstIndexToUse, offsetToAddToEachIndex);
		}
	}
}