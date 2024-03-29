#ifndef DESCRIPTORPOOL_H
#define DESCRIPTORPOOL_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <set>
#include <iostream>
#include <vector>
#include <array>

#include "../device/logicaldevice.h"

class DescriptorPool {
public:
	DescriptorPool();
	~DescriptorPool();

	// create a descriptor pool for the descriptor sets of uniform buffers and texture samplers
	void createDescriptorPool(LogicalDevice& logicaldevice, uint32_t maxFramesinFlight);

	void destroyDescriptorPool(LogicalDevice& logicaldevice);

	VkDescriptorPool vkDescriptorPool; // pool of descriptor sets

private:

};

#endif
