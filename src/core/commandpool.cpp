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

// creates a vulkan command pool
void CommandPool::createCommandBuffers(uint32_t maxFramesinFlight, LogicalDevice& logicaldevice) {
    vkCommandBuffers.resize(maxFramesinFlight);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = vkCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)vkCommandBuffers.size();

    if (vkAllocateCommandBuffers(logicaldevice.device, &allocInfo, vkCommandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

// writes commands into the command buffer
void CommandPool::startRecordCommandBuffer(uint32_t bufferIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(vkCommandBuffers[bufferIndex], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }
}

void CommandPool::endRecordCommandBuffer(uint32_t bufferIndex) {
    if (vkEndCommandBuffer(vkCommandBuffers[bufferIndex]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

// also handles command buffers allocated to pool
void CommandPool::destroyCommandPool(LogicalDevice& logicaldevice) {
    vkDestroyCommandPool(logicaldevice.device, vkCommandPool, nullptr);
}