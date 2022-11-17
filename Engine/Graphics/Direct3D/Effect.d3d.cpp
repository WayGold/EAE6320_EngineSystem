#include "../Effect.h"
#include "../sContext.h"

void eae6320::Graphics::Effect::Effect::init(string vertShader, string fragShader) {

	auto result = eae6320::Results::Success;

	if (!(result = eae6320::Graphics::cShader::Load(vertShader,
		m_vertexShader, eae6320::Graphics::eShaderType::Vertex)))
	{
		EAE6320_ASSERTF(false, "Can't initialize shading data without vertex shader");
		return;
	}
	if (!(result = eae6320::Graphics::cShader::Load(fragShader,
		m_fragmentShader, eae6320::Graphics::eShaderType::Fragment)))
	{
		EAE6320_ASSERTF(false, "Can't initialize shading data without fragment shader");
		return;
	}
	{
		constexpr auto renderStateBits = []
		{
			uint8_t renderStateBits = 0;

			eae6320::Graphics::RenderStates::DisableAlphaTransparency(renderStateBits);
			eae6320::Graphics::RenderStates::DisableDepthTesting(renderStateBits);
			eae6320::Graphics::RenderStates::DisableDepthWriting(renderStateBits);
			eae6320::Graphics::RenderStates::DisableDrawingBothTriangleSides(renderStateBits);
			eae6320::Graphics::RenderStates::EnableDepthTesting(renderStateBits);
			eae6320::Graphics::RenderStates::EnableDepthWriting(renderStateBits);

			return renderStateBits;
		}();
		if (!(result = m_renderState.Initialize(renderStateBits)))
		{
			EAE6320_ASSERTF(false, "Can't initialize shading data without render state");
			return;
		}
	}
}

eae6320::Graphics::Effect::Effect::~Effect() {
	if (m_vertexShader)
	{
		m_vertexShader->DecrementReferenceCount();
		m_vertexShader = nullptr;
	}
	if (m_fragmentShader)
	{
		m_fragmentShader->DecrementReferenceCount();
		m_fragmentShader = nullptr;
	}
}

void eae6320::Graphics::Effect::Effect::Bind() {
	// Bind the shading data
	{
		auto* const direct3dImmediateContext = sContext::g_context.direct3dImmediateContext;
		EAE6320_ASSERT(direct3dImmediateContext);

		{
			constexpr ID3D11ClassInstance* const* noInterfaces = nullptr;
			constexpr unsigned int interfaceCount = 0;
			// Vertex shader
			{
				EAE6320_ASSERT((m_vertexShader != nullptr) && (m_vertexShader->m_shaderObject.vertex != nullptr));
				direct3dImmediateContext->VSSetShader(m_vertexShader->m_shaderObject.vertex, noInterfaces, interfaceCount);
			}
			// Fragment shader
			{
				EAE6320_ASSERT((m_fragmentShader != nullptr) && (m_fragmentShader->m_shaderObject.vertex != nullptr));
				direct3dImmediateContext->PSSetShader(m_fragmentShader->m_shaderObject.fragment, noInterfaces, interfaceCount);
			}
		}
		// Render state
		{
			m_renderState.Bind();
		}
	}
}