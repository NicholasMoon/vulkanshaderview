#ifndef COMMANDBUFFER_H
#define COMMANDBUFFER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "device/physicaldevice.h"
#include "device/logicaldevice.h"
#include "core/commandpool.h"

VkCommandBuffer beginSingleTimeCommands(CommandPool& commandpool, LogicalDevice& logicaldevice);

void endSingleTimeCommands(VkCommandBuffer commandBuffer, CommandPool& commandpool, LogicalDevice& logicaldevice);

#endif