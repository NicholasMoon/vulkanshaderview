#ifndef IMAGE_H
#define IMAGE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdint>

#include "physicaldevice.h"
#include "logicaldevice.h"

class Image {
public:
	Image();
	~Image();

	// create vulkan image
	void createImage(uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, LogicalDevice& logicaldevice);

	void copyBufferToImage(VkBuffer buffer);

	// function to synchronize transition between staging buffer and vulkan image
	void transitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

	// creates an image view
	VkImageView createImageView(VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, LogicalDevice& logicaldevice);

	VkImage vkImage; // handle for vulkan image
	VkDeviceMemory vkImageMemory; // handle for memory of image
	VkImageView vkImageView; // handle for image view
	VkImageUsageFlags vkUsageFlags; // handle for image usage type
	VkImageTiling vkTiling; // handle for image tiling mode
	VkMemoryPropertyFlags vkMemoryPropertyFlags; // handle for image memory properties

	uint32_t width, height, channels;
	
private:
	
};

#endif

