#version 450

layout(binding = 0) uniform UBO_PerRenderPass {
    mat4 view_projection;
	vec4 camera_position;
} ubo_renderpass;

layout (binding = 1) uniform samplerCube cubeMapTexture;

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
	vec3 textureIndex = fragPos;
	//textureIndex.x = textureIndex.x;
//textureIndex.z = - textureIndex.z;
	vec3 envColor = texture(cubeMapTexture, textureIndex).rgb;
	
    envColor = envColor / (envColor + vec3(1.0));

    outColor = vec4(envColor, 1.0);
}