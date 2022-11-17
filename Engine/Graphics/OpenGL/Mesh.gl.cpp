#include "../Mesh.h"

void eae6320::Graphics::Mesh::Mesh::init(VertexFormats::sVertex_mesh points[], uint16_t triangles[],
	unsigned int i_vertexCount, unsigned int i_indexCount) {
	auto result = eae6320::Results::Success;

	m_vertexCount = i_vertexCount;
	m_indexCount = i_indexCount;

	// Create a vertex array object and make it active
	{
		constexpr GLsizei arrayCount = 1;
		glGenVertexArrays(arrayCount, &m_vertexArrayId);
		const auto errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			glBindVertexArray(m_vertexArrayId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind a new vertex array: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				return;
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused vertex array ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return;
		}
	}

	// Create a vertex buffer object
	{
		constexpr GLsizei bufferCount = 1;
		glGenBuffers(bufferCount, &m_vertexBufferId);
		const auto errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind a new vertex buffer: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				return;
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused vertex buffer ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return;
		}
	}

	// Assign the data to the buffer
	{
		auto bufferSize = m_vertexCount * sizeof(points[0]);
		EAE6320_ASSERT(bufferSize <= (unsigned int)std::numeric_limits<GLsizeiptr>::max());

		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(bufferSize), reinterpret_cast<GLvoid*>(points),
			// In our class we won't ever read from the buffer
			GL_STATIC_DRAW);

		const auto errorCode = glGetError();

		if (errorCode != GL_NO_ERROR)
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to allocate the vertex buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return;
		}
	}

	// Create a index buffer object
	{
		glGenBuffers(1, &m_indexBufferId);
		const auto errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind a new index buffer: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				return;
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused index buffer ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return;
		}
	}

	// Assign the data to the buffer
	{
		auto bufferSize = m_indexCount * sizeof(triangles[0]);
		EAE6320_ASSERT(bufferSize <= (unsigned int)std::numeric_limits<GLsizeiptr>::max());

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(bufferSize), triangles, GL_STATIC_DRAW);

		const auto errorCode = glGetError();

		if (errorCode != GL_NO_ERROR)
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to allocate the index buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return;
		}
	}

	// Initialize vertex format
	{
		// The "stride" defines how large a single vertex is in the stream of data
		// (or, said another way, how far apart each position element is)
		constexpr auto stride = static_cast<GLsizei>(sizeof(eae6320::Graphics::VertexFormats::sVertex_mesh));

		// Position (0)
		// 3 floats == 12 bytes
		// Offset = 0
		{
			constexpr GLuint vertexElementLocation = 0;
			constexpr GLint elementCount = 3;
			constexpr GLboolean notNormalized = GL_FALSE;	// The given floats should be used as-is
			glVertexAttribPointer(vertexElementLocation, elementCount, GL_FLOAT, notNormalized, stride,
				reinterpret_cast<GLvoid*>(offsetof(eae6320::Graphics::VertexFormats::sVertex_mesh, x)));
			const auto errorCode = glGetError();

			if (errorCode == GL_NO_ERROR)
			{
				glEnableVertexAttribArray(vertexElementLocation);
				const GLenum errorCode = glGetError();

				if (errorCode != GL_NO_ERROR)
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to enable the POSITION vertex attribute at location %u: %s",
						vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					return;
				}
			}
			else
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to set the POSITION vertex attribute at location %u: %s",
					vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				return;
			}
		}
	}
}

eae6320::Graphics::Mesh::Mesh::~Mesh()
{
	auto result = Results::Success;

	// Geometry
	{
		if (m_vertexArrayId != 0)
		{
			// Make sure that the vertex array isn't bound
			{
				// Unbind the vertex array
				glBindVertexArray(0);
				const auto errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					if (result)
					{
						result = Results::Failure;
					}
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					Logging::OutputError("OpenGL failed to unbind all vertex arrays before cleaning up geometry: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
				}
			}
			constexpr GLsizei arrayCount = 1;
			glDeleteVertexArrays(arrayCount, &m_vertexArrayId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				if (result)
				{
					result = Results::Failure;
				}
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				Logging::OutputError("OpenGL failed to delete the vertex array: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
			}
			m_vertexArrayId = 0;
		}
		if (m_vertexBufferId != 0)
		{
			constexpr GLsizei bufferCount = 1;
			glDeleteBuffers(bufferCount, &m_vertexBufferId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				if (result)
				{
					result = Results::Failure;
				}
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				Logging::OutputError("OpenGL failed to delete the vertex buffer: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
			}
			m_vertexBufferId = 0;
		}
		if (m_indexBufferId != 0) {
			constexpr GLsizei bufferCount = 1;
			glDeleteBuffers(bufferCount, &m_indexBufferId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				if (result)
				{
					result = Results::Failure;
				}
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				Logging::OutputError("OpenGL failed to delete the index buffer: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
			}
			m_indexBufferId = 0;
		}
	}
}

void eae6320::Graphics::Mesh::Mesh::Draw()
{
	// Draw the geometry
	{
		// Bind a specific vertex buffer to the device as a data source
		{
			EAE6320_ASSERT(m_vertexArrayId != 0);
			glBindVertexArray(m_vertexArrayId);
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		}

		// Render triangles from the currently-bound vertex buffer
		{
			// The mode defines how to interpret multiple vertices as a single "primitive";
			// a triangle list is defined
			// (meaning that every primitive is a triangle and will be defined by three vertices)
			constexpr GLenum mode = GL_TRIANGLES;
			// It's possible to start rendering primitives in the middle of the stream
			const GLvoid* const offset = 0;
			glDrawElements(mode, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_SHORT, offset);
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		}

		// Render triangles from the currently-bound vertex buffer
		//{
		//	// The mode defines how to interpret multiple vertices as a single "primitive";
		//	// a triangle list is defined
		//	// (meaning that every primitive is a triangle and will be defined by three vertices)
		//	constexpr GLenum mode = GL_TRIANGLES;
		//	// As of this comment only a single triangle is drawn
		//	// (you will have to update this code in future assignments!)
		//	constexpr unsigned int triangleCount = 2;
		//	constexpr unsigned int vertexCountPerTriangle = 3;
		//	auto vertexCountToRender = m_triangles.size() * m_vertices.size();
		//	// It's possible to start rendering primitives in the middle of the stream
		//	constexpr unsigned int indexOfFirstVertexToRender = 0;
		//	glDrawArrays(mode, indexOfFirstVertexToRender, vertexCountToRender);
		//	EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		//}

	}
}