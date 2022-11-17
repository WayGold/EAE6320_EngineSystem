#include "Effect.h"

eae6320::cResult eae6320::Graphics::Effect::Effect::createEffect(Effect*& o_ptr, string vertShader, string fragShader)
{
	cResult result = Results::Success;

	o_ptr = new Effect();
	o_ptr->init(vertShader, fragShader);

	if (!o_ptr)
		result = Results::Failure;

	return result;
}