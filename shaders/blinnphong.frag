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

	vec3 n = normalize(fragNor);
	vec3 fragToLightPos = ubo.lightPos.xyz - fragPos;
	vec3 fragToCamera = ubo.camPos.xyz - fragPos;
	
	float distanceSq = dot(fragToLightPos, fragToLightPos);
	float inverseDistanceSq = 1.0f / distanceSq;
	
	vec3 H = normalize((normalize(fragToLightPos) + normalize(fragToCamera)) / 2.0f);
	
	vec3 Lo = vec3(0,0,0);
	
	float lightDirVSNormal = dot(n, normalize(fragToLightPos));
	
	if (lightDirVSNormal > 0.0f) {
		vec3 textureColor = vec3(texture(texSampler, fragUV));
		
		vec3 diffuse = ubo.lightCol.rgb * lightDirVSNormal * textureColor;
		
		float specularIntensity = pow(max(dot(n, H), 0.0), ubo.roughness * 64.0);
		vec3 specular = ubo.lightCol.rgb * specularIntensity;
		
		Lo = (diffuse + specular) * inverseDistanceSq;
	}
	
	// reinhard (HDR)
	Lo = Lo / (Lo + vec3(1.0f, 1.0f, 1.0f));

	// gamma correct
	Lo = pow(Lo, vec3(GAMMA, GAMMA, GAMMA));
	
	outColor = vec4(Lo,1);
}