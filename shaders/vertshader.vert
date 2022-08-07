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

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNor;
layout(location = 2) out vec3 fragTan;
layout(location = 3) out vec3 fragBit;
layout(location = 4) out vec2 fragUV;

void main() {
    fragNor = normalize(mat3(ubo_primitve.modelInvTr) * inNor);
	fragTan = normalize(mat3(ubo_primitve.model) * inTan);
    fragBit = normalize(mat3(ubo_primitve.model) * inBit);

	vec4 modelPosition = ubo_primitve.model * vec4(inPos, 1.0);
	
	fragPos = modelPosition.xyz;
    gl_Position = ubo_renderpass.view_projection * modelPosition;
    fragUV = inUV;
}