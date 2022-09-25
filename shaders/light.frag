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

layout(location = 0) out vec4 outColor;

void main() {
	float intensity = length(ubo_primitve.lightColor.rgb);
	if (intensity > 3.0) {
		vec3 normalized = normalize(ubo_primitve.lightColor.rgb);
		outColor = vec4(normalized,1);
	}
	else {
		outColor = vec4(ubo_primitve.lightColor.rgb,1);
	}
	
}