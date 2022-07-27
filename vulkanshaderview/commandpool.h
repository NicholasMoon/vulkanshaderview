#ifndef COMMANDPOOL_H
#define COMMANDPOOL_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "physicaldevice.h"
#include "logicaldevice.h"
#include "surface.h"

class CommandPool {
public:
	CommandPool();
	~CommandPool();

	// creates the vulkan command pool
	void createCommandPool(PhysicalDevice& physicaldevice, LogicalDevice& logicaldevice, Surface& surface);

	VkCommandPool vkCommandPool; // handle for vulkan command pool
private:

};

#endif

