#ifndef INCLUDES_H
#define INCLUDES_H


// GLFW
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


// GLM
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>


// TINY OBJ
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>


// STB
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>



#endif