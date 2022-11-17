#include "BufferManager.h"

void eae6320::Graphics::BufferManager::updateConstantBuffer(sDataRequiredToRenderAFrame* const s_dataBeingRenderedByRenderThread,
	eae6320::Graphics::cConstantBuffer& s_constantBuffer_frame)
{
	EAE6320_ASSERT(s_dataBeingRenderedByRenderThread);

	// Update the frame constant buffer
	{
		// Copy the data from the system memory that the application owns to GPU memory
		auto& constantData_frame = s_dataBeingRenderedByRenderThread->constantData_frame;
		s_constantBuffer_frame.Update(&constantData_frame);
	}
}