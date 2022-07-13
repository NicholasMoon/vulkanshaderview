#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragCam;

layout(location = 0) out vec4 outColor;



void main() {
    vec3 diffuseColor = vec3(texture(texSampler, fragTexCoord));
	float diffuseTerm = dot(normalize(fragNormal), normalize(vec3(1,-.5,-.5)));
	diffuseTerm = clamp(diffuseTerm, 0, 1);
	float ambientTerm = 0.00;
	float lightIntensity = diffuseTerm + ambientTerm;
	
	float specularIntensity = max(pow(dot(normalize((normalize(fragCam) + normalize(vec3(1,-.5,-.5))) / 2), normalize(fragNormal)), 42), 0);
	outColor = vec4(diffuseColor.rgb * lightIntensity + specularIntensity,1);
}