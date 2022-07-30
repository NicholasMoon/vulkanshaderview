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

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 4) in vec3 fragPos;

layout(location = 0) out vec4 outColor;


const float GAMMA = 0.45454545454f;


void main() {

	vec3 fragToLightPos = ubo.lightPos.rgb - fragPos;
	float distanceSq = dot(fragToLightPos, fragToLightPos);
	float inverseDistanceSq = 1.0f / distanceSq;

    vec3 textureColor = vec3(texture(texSampler, fragTexCoord));
	float diffuseTerm = dot(normalize(fragNormal), normalize(vec3(ubo.lightPos)));
	diffuseTerm = clamp(diffuseTerm, 0, 1);
	float ambientTerm = 0.00;
	float lightIntensity = diffuseTerm + ambientTerm;
	
	float specularIntensity = max(pow(dot(normalize((normalize(vec3(ubo.camPos)) + normalize(vec3(ubo.lightPos))) / 2), normalize(fragNormal)), 42), 0);
	vec3 combinedColor = ubo.lightCol.rgb * (textureColor.rgb * lightIntensity) * inverseDistanceSq;
	
	
	
	
	// reinhard (HDR)
    combinedColor = combinedColor / (combinedColor + vec3(1.0f, 1.0f, 1.0f));

    // gamma correct
    combinedColor = pow(combinedColor, vec3(GAMMA, GAMMA, GAMMA));
	
	outColor = vec4(combinedColor,1);
}