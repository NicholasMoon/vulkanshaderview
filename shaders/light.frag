#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
	mat4 modelInvTr;
	vec4 camPos;
	vec4 lightPos;
	vec4 lightCol;
	float metallic;
	float roughness;
} ubo;

layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 2) uniform sampler2D texSampler2;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNor;
layout(location = 2) in vec3 fragTan;
layout(location = 3) in vec3 fragBit;
layout(location = 4) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

const float PI = 3.14159f;
const float GAMMA = 0.45454545454f;


void main() {
	float intensity = length(ubo.lightCol.rgb);
	if (intensity > 3.0) {
		vec3 normalized = normalize(ubo.lightCol.rgb);
		outColor = vec4(normalized,1);
	}
	else {
		outColor = vec4(ubo.lightCol.rgb,1);
	}
	
}