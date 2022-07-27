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

	// creates a vulkan command pool
	void createCommandBuffers(uint32_t maxFramesinFlight, LogicalDevice& logicaldevice);

	// writes commands into the command buffer
	void startRecordCommandBuffer(uint32_t bufferIndex);
	void endRecordCommandBuffer(uint32_t bufferIndex);

	// destroy vulkan command pool and each command buffer in this pool
	void destroyCommandPool(LogicalDevice& logicaldevice);

	VkCommandPool vkCommandPool; // handle for vulkan command pool
	std::vector<VkCommandBuffer> vkCommandBuffers; // handle for vulkan command buffers belonging to this pool

private:

};

#endif

