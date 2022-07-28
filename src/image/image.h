#ifndef IMAGE_H
#define IMAGE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdint>

#include "../device/physicaldevice.h"
#include "../device/logicaldevice.h"

#include "../commandbuffer.h"
#include "../core/commandpool.h"

class Image {
public:
	Image();
	~Image();

	// create vulkan image
	void createImage(uint32_t mipLevels, VkSampleCountFlagBits numSamples, LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice);

	void copyBufferToImage(VkBuffer buffer, CommandPool& commandpool, LogicalDevice& logicaldevice);

	// function to synchronize transition between staging buffer and vulkan image
	void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, CommandPool& commandpool, LogicalDevice& logicaldevice);

	// creates an image view
	VkImageView createImageView(uint32_t mipLevels, LogicalDevice& logicaldevice);

	bool hasStencilComponent();

	// destroys vulkan components related to Image
	virtual void destroyImage(LogicalDevice& logicaldevice);




	// Vulkan image-related objects
	VkImage vkImage; // handle for vulkan image
	VkDeviceMemory vkImageMemory; // handle for memory of image
	VkImageView vkImageView; // handle for image view
	VkImageUsageFlags vkUsageFlags; // handle for image usage type
	VkImageAspectFlags vkImageAspectFlags; // handle for image aspect flags
	VkImageTiling vkTiling; // handle for image tiling mode
	VkFormat vkFormat; // handle for image format
	VkFormatFeatureFlags vkFormatFeatureFlags; // handle for format feature flags
	VkMemoryPropertyFlags vkMemoryPropertyFlags; // handle for image memory properties

	uint32_t width, height, channels;

private:
	
};

#endif

