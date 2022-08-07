#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
	vec4 lightCol;
} ubo;

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNor;
layout(location = 2) in vec3 inTan;
layout(location = 3) in vec3 inBit;
layout(location = 4) in vec2 inUV;

void main() {	
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPos, 1.0);
}