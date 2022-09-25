#version 450

struct PointLight {    
    vec4 pos;
	vec4 col;
}; 

layout(binding = 0) uniform UBO_PerRenderPass {
    mat4 view_projection;
	vec4 camera_position;
} ubo_renderpass;

layout(binding = 1) uniform UBO_PerPrimitive {
    mat4 model;
	mat4 modelInvTr;
	PointLight pointlights[10];
	float metallic;
	float roughness;
	vec4 lightColor;
} ubo_primitve;

layout(binding = 2) uniform sampler2D texSampler;

layout(binding = 3) uniform sampler2D texSampler2;

layout (binding = 4) uniform samplerCube cubeMapTexture;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNor;
layout(location = 2) in vec3 fragTan;
layout(location = 3) in vec3 fragBit;
layout(location = 4) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

const float PI = 3.14159f;
const float GAMMA = 0.45454545454f;

void main()
{
	vec3 normal = normalize(fragNor);
	//normal.y *= -1.0f;
	vec4 textureSample = texture(cubeMapTexture, normal);
	
	outColor = vec4(textureSample.rgb,1);
}