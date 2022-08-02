#include "primitive.h"

Primitive::Primitive() : m_geometry(), m_texture(), m_vertexbuffer(), m_indexbuffer(), m_descriptorsets() {};


void Primitive::loadGeometryFromOBJ(std::string& filePath) {
    m_geometry.loadOBJ(filePath);
}

void Primitive::toString() {
    m_geometry.toString();
}

// creates vertex buffer to store vert data
void Primitive::createVertexBuffer(LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice, CommandPool& commandpool) {
    m_vertexbuffer.vkBufferSize = sizeof(m_geometry.vertices[0]) * m_geometry.vertices.size();

    // create staging buffer
    DataBuffer stagingBuffer(m_vertexbuffer.vkBufferSize);
    stagingBuffer.createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, logicaldevice, physicaldevice);

    void* data;
    vkMapMemory(logicaldevice.device, stagingBuffer.vkBufferMemory, 0, m_vertexbuffer.vkBufferSize, 0, &data);
    memcpy(data, m_geometry.vertices.data(), (size_t)m_vertexbuffer.vkBufferSize);
    vkUnmapMemory(logicaldevice.device, stagingBuffer.vkBufferMemory);

    // create actual buffer
    m_vertexbuffer.createBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, logicaldevice, physicaldevice);

    m_vertexbuffer.copyBuffer(stagingBuffer.vkBuffer, m_vertexbuffer.vkBuffer, m_vertexbuffer.vkBufferSize, logicaldevice, commandpool);

    vkDestroyBuffer(logicaldevice.device, stagingBuffer.vkBuffer, nullptr);
    vkFreeMemory(logicaldevice.device, stagingBuffer.vkBufferMemory, nullptr);
}

// creates index buffer to store vert index data
void Primitive::createIndexBuffer(LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice, CommandPool& commandpool) {
    m_indexbuffer.vkBufferSize = sizeof(m_geometry.indices[0]) * m_geometry.indices.size();

    DataBuffer stagingBuffer(m_indexbuffer.vkBufferSize);
    stagingBuffer.createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, logicaldevice, physicaldevice);

    void* data;
    vkMapMemory(logicaldevice.device, stagingBuffer.vkBufferMemory, 0, m_indexbuffer.vkBufferSize, 0, &data);
    memcpy(data, m_geometry.indices.data(), (size_t)m_indexbuffer.vkBufferSize);
    vkUnmapMemory(logicaldevice.device, stagingBuffer.vkBufferMemory);

    m_indexbuffer.createBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, logicaldevice, physicaldevice);

    m_indexbuffer.copyBuffer(stagingBuffer.vkBuffer, m_indexbuffer.vkBuffer, m_indexbuffer.vkBufferSize, logicaldevice, commandpool);

    vkDestroyBuffer(logicaldevice.device, stagingBuffer.vkBuffer, nullptr);
    vkFreeMemory(logicaldevice.device, stagingBuffer.vkBufferMemory, nullptr);
}

// create buffers for the uniform variables (one for each frame in flight)
void Primitive::createUniformBuffers(uint32_t maxFramesinFlight, LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice) {
    m_uniformbuffers.resize(maxFramesinFlight);

    for (size_t j = 0; j < maxFramesinFlight; ++j) {
        m_uniformbuffers[j].vkBufferSize = sizeof(UniformBufferObject);

        m_uniformbuffers[j].createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, logicaldevice, physicaldevice);
    }
}


Primitive::~Primitive() {};