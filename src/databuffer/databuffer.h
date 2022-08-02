#ifndef DATABUFFER_H
#define DATABUFFER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "../commandbuffer.h"
#include "../ubo.h"
#include "../core/commandpool.h"

class DataBuffer {
public:
	DataBuffer();
	DataBuffer(VkDeviceSize buffersize);
	~DataBuffer();

	// create a buffer and allocate memory
	void createBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice);

	// copies memory from one buffer to another
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, LogicalDevice& logicaldevice, CommandPool& commandPool);

	void destroyBuffer(LogicalDevice& logicaldevice);

	VkBuffer vkBuffer; // handle for the vert buffer
	VkDeviceMemory vkBufferMemory; // handle for vert memory buffer
	VkDeviceSize vkBufferSize; // amount of bytes allocated to buffer

private:

};

#endif
