#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
	vec4 lightCol;
} ubo;

layout(location = 0) out vec4 outColor;

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