#pragma once

#include "sContext.h"
#include "cConstantBuffer.h"
#include "ConstantBufferFormats.h"

#include <vector>
#include <utility>

using namespace eae6320::Graphics;

namespace eae6320
{
	namespace Graphics
	{
		// This struct's data is populated at submission time;
		// it must cache whatever is necessary in order to render a frame
		struct sDataRequiredToRenderAFrame
		{
			eae6320::Graphics::ConstantBufferFormats::sFrame constantData_frame;
			eae6320::Graphics::ConstantBufferFormats::sDrawCall constantData_drawCall[10];

			float bgColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
			unsigned int gameObjectCount;

			//std::pair<Mesh::Mesh*, Effect::Effect*>* pairArr;
			Mesh::Mesh* meshArr[10];
			Effect::Effect* effectArr[10];
		};

		namespace BufferManager {
			void updateConstantBuffer(sDataRequiredToRenderAFrame* const s_dataBeingRenderedByRenderThread,
				eae6320::Graphics::cConstantBuffer &s_constantBuffer_frame);
			void presentBackBuffer();
		}
	}
}