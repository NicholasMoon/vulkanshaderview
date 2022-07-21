#ifndef DEPTHBUFFER_H
#define DEPTHBUFFER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <string>
#include <set>
#include <iostream>
#include <vector>

#include "image.h"

#include "physicaldevice.h"
#include "logicaldevice.h"

class DepthBuffer : Image {
public:
	DepthBuffer();
	~DepthBuffer();

	// create vulkan structures related to depth buffer
	void createDepthResources();

	bool hasStencilComponent(VkFormat format);

	// find best format for depth buffer
	VkFormat findDepthFormat();

	// find supported image formats of the physical device
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);


private:

};

#endif