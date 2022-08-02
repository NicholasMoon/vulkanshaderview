#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>

#include "light.h"

class PointLight : Light {
public:
	PointLight();
	~PointLight();

private:

};


#endif
