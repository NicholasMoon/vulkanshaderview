#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
	mat4 modelInvTr;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragNormal;
layout(location = 3) out vec3 fragCam;

void main() {
	mat3 invTranspose = mat3(ubo.modelInvTr);
    fragNormal = normalize(invTranspose * inNormal);

	vec4 modelPosition = ubo.model * vec4(inPosition, 1.0);
	//vec4 eye = vec4(ubo.view[3][0], ubo.view[3][1], ubo.view[3][2], 1);
	vec4 eye = vec4(2, 2, 2, 1);
	fragCam = vec3(eye - modelPosition);
	

    gl_Position = ubo.proj * ubo.view * modelPosition;
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}