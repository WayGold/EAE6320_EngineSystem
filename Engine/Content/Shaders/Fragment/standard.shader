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
		1.0, 1.0, 1.0,
		// Alpha (opacity)
		1.0 );
}
