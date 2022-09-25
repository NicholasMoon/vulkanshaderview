#ifndef CUBEMAP_H
#define CUBEMAP_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>



#include <string>
#include <set>
#include <iostream>
#include <vector>
#include <stdint.h>

#include "image.h"

#include "../device/physicaldevice.h"
#include "../device/logicaldevice.h"
#include "../core/commandpool.h"
#include "../databuffer/databuffer.h"

class CubeMap : Image {
public:
	CubeMap();
	~CubeMap();

	// creates a texture from an image
	void createCubeMapImage(std::string texturepath, LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice, CommandPool& commandpool);

	void createCubeMapImageView(LogicalDevice& logicaldevice);

	// create texture sampler used to access image texels in fragment shader
	void createCubeMapSampler(LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice);

	// destroys vulkan components related to Image
	virtual void destroyImage(LogicalDevice& logicaldevice);

	uint32_t mipLevels;			// number of mip levels (calculated from width and height of image)
	VkSampler vkTextureSampler;	// handle for texture sampler
	std::string directoryPath;

	VkImage get_vkImage() { return vkImage; }
	VkDeviceMemory get_vkImageMemory() { return vkImageMemory; }
	VkImageView get_vkImageView() { return vkImageView; }
	VkFormat get_vkFormat() { return vkFormat; }

private:
	
};


#endif
