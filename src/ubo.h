#ifndef UBO_H
#define UBO_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    alignas(16) glm::mat4 modelInvTr;
    alignas(16) glm::vec4 camPos;
    alignas(16) glm::vec4 lightPos;
    alignas(16) glm::vec4 lightCol;
    alignas(4)  glm::float32 metallic;
    alignas(4)  glm::float32 roughness;
};

#endif