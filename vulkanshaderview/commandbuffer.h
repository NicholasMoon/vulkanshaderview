#ifndef COMMANDBUFFER_H
#define COMMANDBUFFER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "physicaldevice.h"
#include "logicaldevice.h"
#include "commandpool.h"

VkCommandBuffer beginSingleTimeCommands(CommandPool& commandpool, LogicalDevice& logicaldevice);

void endSingleTimeCommands(VkCommandBuffer commandBuffer, CommandPool& commandpool, LogicalDevice& logicaldevice);

#endif