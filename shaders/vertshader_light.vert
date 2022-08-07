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



layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNor;
layout(location = 2) in vec3 inTan;
layout(location = 3) in vec3 inBit;
layout(location = 4) in vec2 inUV;

void main() {	
    gl_Position = ubo_renderpass.view_projection * ubo_primitve.model * vec4(inPos, 1.0);
}