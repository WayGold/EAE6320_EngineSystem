/*
	This is the standard fragment shader

	A fragment shader is responsible for telling the GPU what color a specific fragment should be
*/

#include <Shaders/shaders.inc>


// Entry Point
//============

frag_main
{
	// Output solid white
	o_color = float4(
		// RGB (color)
		0.0, 0.0, 0.0,
		// Alpha (opacity)
		1.0 );
		
	float sin_pad = abs(sin(g_elapsedSecondCount_simulationTime));
	float cos_pad = abs(cos(g_elapsedSecondCount_simulationTime));
	
	o_color.r = cos_pad;
	o_color.g = sin_pad;
	o_color.b = cos_pad;
}
