#version 450

layout(binding = 0) uniform UBO_PerRenderPass {
    mat4 view_projection;
	vec4 camera_position;
} ubo_renderpass;

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
	vec3 newPos = inPos;
	//newPos.x = -newPos.x;
    fragPos = newPos;
    vec4 clipPos = ubo_renderpass.view_projection * vec4(newPos, 1.0);

    gl_Position = clipPos.xyww;
}