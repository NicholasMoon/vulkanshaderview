#ifndef MSAABUFFER_H
#define MSAABUFFER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <set>
#include <iostream>
#include <vector>

#include "image.h"

#include "physicaldevice.h"
#include "logicaldevice.h"
#include "swapchain.h"

class Swapchain;

class MSAABuffer : Image {
public:
	MSAABuffer();
	~MSAABuffer();

	// create image resources for msaa buffer
	void createColorResources(Swapchain& swapchain, LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice);

	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT; // max supported msaa samples

	VkImage get_vkImage() { return vkImage; }
	VkDeviceMemory get_vkImageMemory() { return vkImageMemory; }
	VkImageView get_vkImageView() { return vkImageView; }
	VkFormat get_vkFormat() { return vkFormat; }

private:

};

#endif