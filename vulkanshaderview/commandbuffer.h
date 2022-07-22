#ifndef COMMANDBUFFER_H
#define COMMANDBUFFER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "physicaldevice.h"
#include "logicaldevice.h"

// create a buffer and allocate memory
void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory, LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice);

VkCommandBuffer beginSingleTimeCommands(VkCommandPool& commandPool, LogicalDevice& logicaldevice);

void endSingleTimeCommands(VkCommandBuffer commandBuffer, VkCommandPool& commandPool, LogicalDevice& logicaldevice);

#endif