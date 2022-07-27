#include "commandpool.h"

CommandPool::CommandPool() {}

CommandPool::~CommandPool() {}

// creates the vulkan command pool
void CommandPool::createCommandPool(PhysicalDevice& physicaldevice, LogicalDevice& logicaldevice, Surface &surface) {
    QueueFamilyIndices queueFamilyIndices = physicaldevice.findQueueFamilies(surface);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // reset every frame
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value(); // for graphics commands

    if (vkCreateCommandPool(logicaldevice.device, &poolInfo, nullptr, &vkCommandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}