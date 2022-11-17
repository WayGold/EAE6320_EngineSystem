/*
	This file should be #included by all shaders
*/

// Version Information
//====================

#if defined( EAE6320_PLATFORM_GL )

	// GLSL shaders require the version to be #defined before anything else in the shader
	#version 420

#endif

// Constant Buffers
//=================

#if defined( EAE6320_PLATFORM_D3D )

	#define DeclareConstantBuffer( i_name, i_id ) cbuffer i_name : register( b##i_id )
	#define DeclareDrawCallBuffer( i_name, i_id ) cbuffer i_name : register( b##i_id )

#elif defined( EAE6320_PLATFORM_GL )

	#define DeclareConstantBuffer( i_name, i_id ) layout( std140, binding = i_id ) uniform i_name
	#define DeclareDrawCallBuffer( i_name, i_id ) layout( std140, binding = i_id ) uniform i_name

#endif


// Types
//=================

#if defined( EAE6320_PLATFORM_GL )

	#define float2 vec2
    #define float3 vec3
    #define float4 vec4
    #define float2x2 mat2
    #define float3x3 mat3
    #define float4x4 mat4
	#define o_vertexPosition_projected gl_Position

#endif

DeclareConstantBuffer(g_constantBuffer_frame, 0)
{
	float4x4 g_transform_worldToCamera;
	float4x4 g_transform_cameraToProjected;

	float g_elapsedSecondCount_systemTime;
	float g_elapsedSecondCount_simulationTime;
	// For float4 alignment
	float2 g_padding;
};

DeclareDrawCallBuffer(g_constantBuffer_drawCall, 2)
{
  float4x4 g_transform_localToWorld;
};

// Functions
// ================

#if defined( EAE6320_PLATFORM_GL )

	#define mul(i_matrix, i_vector) i_matrix * i_vector

	#define frag_main out vec4 o_color; \
	void main() \

	#define vert_main layout( location = 0 ) in vec3 i_vertexPosition_local; \
	void main() \

#endif

#if defined( EAE6320_PLATFORM_D3D )

	#define frag_main \
	void main( \
		in const float4 i_fragmentPosition : SV_POSITION, \
		out float4 o_color : SV_TARGET \
	)

	#define vert_main \
	void main( \
		in const float3 i_vertexPosition_local : POSITION, \
		out float4 o_vertexPosition_projected : SV_POSITION \
	)

#endif