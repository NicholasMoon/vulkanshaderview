#ifndef DEPTHBUFFER_H
#define DEPTHBUFFER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <set>
#include <iostream>
#include <vector>

#include "image.h"

#include "../device/physicaldevice.h"
#include "../device/logicaldevice.h"
#include "../core/swapchain.h"
#include "../core/commandpool.h"

class Swapchain;

class DepthBuffer : Image {
public:
	DepthBuffer();
	~DepthBuffer();

	// create vulkan structures related to depth buffer
	void createDepthResources(Swapchain& swapchain, VkSampleCountFlagBits msaaSamples, PhysicalDevice& physicaldevice, LogicalDevice& logicaldevice, CommandPool& commandpool);

	// find best format for depth buffer
	VkFormat findDepthFormat(PhysicalDevice& physicaldevice);

	// find supported image formats of the physical device
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, PhysicalDevice& physicaldevice);

	// destroys vulkan components related to Image
	virtual void destroyImage(LogicalDevice& logicaldevice);

	VkImage get_vkImage() { return vkImage; }
	VkDeviceMemory get_vkImageMemory() { return vkImageMemory; }
	VkImageView get_vkImageView() { return vkImageView; }
	VkFormat get_vkFormat() { return vkFormat; }

private:

};

#endif