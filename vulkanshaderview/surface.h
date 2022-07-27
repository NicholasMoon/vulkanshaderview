#ifndef SURFACE_H
#define SURFACE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

#include "vulkaninstance.h"

class Surface {
public:
	Surface();
	~Surface();

	// creates window surface
	void createSurface(VulkanInstance& vulkaninstance, GLFWwindow* window);

	VkSurfaceKHR vkSurface; // Window Surface
private:

};

#endif