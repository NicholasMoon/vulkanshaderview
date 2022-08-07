#ifndef UBO_H
#define UBO_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

struct PointLight_ubo {
    alignas(16) glm::vec4 lightPos;
    alignas(16) glm::vec4 lightCol;
};

struct UBO_PerRenderPass {
    alignas(16) glm::mat4 view_projection;
    alignas(16) glm::vec4 camera_position;
};

struct UBO_PerPrimitive {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 modelInvTr;
    alignas(16) PointLight_ubo pointlights[10];
    alignas(4)  glm::float32 metallic;
    alignas(4)  glm::float32 roughness;
    alignas(16) glm::vec4 lightColor;
};

#endif