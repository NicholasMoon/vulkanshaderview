#ifndef DATABUFFER_H
#define DATABUFFER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "commandbuffer.h"
#include "ubo.h"
#include "commandpool.h"

#include "mesh.h"


class DataBuffer {
public:
	DataBuffer();
	DataBuffer(VkDeviceSize buffersize);
	~DataBuffer();

	// create a buffer and allocate memory
	void createBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice);

	// creates vertex buffer to store vert data
	void createVertexBuffer(Mesh* myMesh, LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice, CommandPool& commandpool);

	// creates index buffer to store vert index data
	void createIndexBuffer(Mesh* myMesh, LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice, CommandPool& commandpool);

	// create buffers for the uniform variables (one for each frame in flight)
	void createUniformBuffer(LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice);

	// copies memory from one buffer to another
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, LogicalDevice& logicaldevice, CommandPool& commandPool);

	VkBuffer vkBuffer; // handle for the vert buffer
	VkDeviceMemory vkBufferMemory; // handle for vert memory buffer
	VkDeviceSize vkBufferSize; // amount of bytes allocated to buffer

private:

};

#endif
