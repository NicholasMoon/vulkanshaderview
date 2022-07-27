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

// creates vertex buffer to store vert data
void DataBuffer::createVertexBuffer(Mesh* myMesh, LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice, CommandPool& commandpool) {
    vkBufferSize = sizeof(myMesh->vertices[0]) * myMesh->vertices.size();

    // create staging buffer
    DataBuffer stagingBuffer(vkBufferSize);
    stagingBuffer.createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, logicaldevice, physicaldevice);

    void* data;
    vkMapMemory(logicaldevice.device, stagingBuffer.vkBufferMemory, 0, vkBufferSize, 0, &data);
    memcpy(data, myMesh->vertices.data(), (size_t)vkBufferSize);
    vkUnmapMemory(logicaldevice.device, stagingBuffer.vkBufferMemory);

    // create actual buffer
    createBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, logicaldevice, physicaldevice);

    copyBuffer(stagingBuffer.vkBuffer, vkBuffer, vkBufferSize, logicaldevice, commandpool);

    vkDestroyBuffer(logicaldevice.device, stagingBuffer.vkBuffer, nullptr);
    vkFreeMemory(logicaldevice.device, stagingBuffer.vkBufferMemory, nullptr);
}

// creates index buffer to store vert index data
void DataBuffer::createIndexBuffer(Mesh* myMesh, LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice, CommandPool& commandpool) {
    vkBufferSize = sizeof(myMesh->indices[0]) * myMesh->indices.size();

    DataBuffer stagingBuffer(vkBufferSize);
    stagingBuffer.createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, logicaldevice, physicaldevice);

    void* data;
    vkMapMemory(logicaldevice.device, stagingBuffer.vkBufferMemory, 0, vkBufferSize, 0, &data);
    memcpy(data, myMesh->indices.data(), (size_t)vkBufferSize);
    vkUnmapMemory(logicaldevice.device, stagingBuffer.vkBufferMemory);

    createBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, logicaldevice, physicaldevice);

    copyBuffer(stagingBuffer.vkBuffer, vkBuffer, vkBufferSize, logicaldevice, commandpool);

    vkDestroyBuffer(logicaldevice.device, stagingBuffer.vkBuffer, nullptr);
    vkFreeMemory(logicaldevice.device, stagingBuffer.vkBufferMemory, nullptr);
}

// create buffers for the uniform variables (one for each frame in flight)
void DataBuffer::createUniformBuffer(LogicalDevice &logicaldevice, PhysicalDevice& physicaldevice) {
    vkBufferSize = sizeof(UniformBufferObject);

    createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, logicaldevice, physicaldevice);
}

// copies memory from one buffer to another
void DataBuffer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, LogicalDevice& logicaldevice, CommandPool& commandpool) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(commandpool, logicaldevice);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer, commandpool, logicaldevice);
}