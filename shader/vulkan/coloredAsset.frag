#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : require

#include "test.glsl"

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec3 inViewVec;
layout (location = 3) in vec3 inLightVec;

layout (location = 0) out vec4 outFragColor;

void main() 
{
	vec3 N = normalize(inNormal);
	vec3 L = normalize(inLightVec);
	vec3 V = normalize(inViewVec);
	vec3 R = reflect(-L, N);
	float diffuse = max(dot(N, L), 0.0);
	vec3 specular = pow(max(dot(R, V), 0.0), 16.0) * vec3(0.75);
	outFragColor = vec4(diffuse * inColor + specular, 1.0);		
}
