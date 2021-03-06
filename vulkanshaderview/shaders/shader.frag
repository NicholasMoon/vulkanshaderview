#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
	mat4 modelInvTr;
	vec3 lightPos;
} ubo;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragCam;
layout(location = 4) in vec3 fragPos;

layout(location = 0) out vec4 outColor;



void main() {

	vec3 fragToLightPos = ubo.lightPos - fragPos;
	float distanceSq = dot(fragToLightPos, fragToLightPos);
	float inverseDistanceSq = 1.0f / distanceSq;

    vec3 diffuseColor = vec3(texture(texSampler, fragTexCoord));
	float diffuseTerm = dot(normalize(fragNormal), normalize(vec3(ubo.lightPos)));
	diffuseTerm = clamp(diffuseTerm, 0, 1);
	float ambientTerm = 0.00;
	float lightIntensity = diffuseTerm + ambientTerm;
	
	float specularIntensity = max(pow(dot(normalize((normalize(fragCam) + normalize(vec3(ubo.lightPos))) / 2), normalize(fragNormal)), 42), 0);
	vec3 combinedColor = (diffuseColor.rgb * lightIntensity + specularIntensity) * inverseDistanceSq;
	outColor = vec4(combinedColor,1);
}