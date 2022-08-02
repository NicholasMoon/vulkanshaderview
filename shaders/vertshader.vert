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

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNor;
layout(location = 2) in vec3 inTan;
layout(location = 3) in vec3 inBit;
layout(location = 4) in vec2 inUV;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNor;
layout(location = 2) out vec3 fragTan;
layout(location = 3) out vec3 fragBit;
layout(location = 4) out vec2 fragUV;

void main() {
	mat3 invTranspose = mat3(ubo.modelInvTr);
    fragNor = normalize(invTranspose * inNor);
	fragTan = normalize(mat3(ubo.model) * inTan);
    fragBit = normalize(mat3(ubo.model) * inBit);

	vec4 modelPosition = ubo.model * vec4(inPos, 1.0);
	
	fragPos = modelPosition.xyz;
    gl_Position = ubo.proj * ubo.view * modelPosition;
    fragUV = inUV;
}