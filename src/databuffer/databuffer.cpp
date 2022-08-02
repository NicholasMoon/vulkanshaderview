#include "databuffer.h"

DataBuffer::DataBuffer() : vkBufferSize(0) {}

DataBuffer::DataBuffer(VkDeviceSize buffersize) : vkBufferSize(buffersize) {}

DataBuffer::~DataBuffer() {}

// create a buffer and allocate memory
void DataBuffer::createBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = vkBufferSize;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(logicaldevice.device, &bufferInfo, nullptr, &vkBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(logicaldevice.device, vkBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties, physicaldevice);

    if (vkAllocateMemory(logicaldevice.device, &allocInfo, nullptr, &vkBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(logicaldevice.device, vkBuffer, vkBufferMemory, 0);
}

// copies memory from one buffer to another
void DataBuffer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, LogicalDevice& logicaldevice, CommandPool& commandpool) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(commandpool, logicaldevice);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer, commandpool, logicaldevice);
}

void DataBuffer::destroyBuffer(LogicalDevice& logicaldevice) {
    // free buffer handle
    vkDestroyBuffer(logicaldevice.device, vkBuffer, nullptr);
    // free  buffer memory
    vkFreeMemory(logicaldevice.device, vkBufferMemory, nullptr);
}