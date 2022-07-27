#ifndef TEXTURE_H
#define TEXTURE_H

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
#include "commandpool.h"
#include "databuffer.h"

class Texture : Image {
public:
	Texture();
	~Texture();

	// creates a texture from an image
	void createTextureImage(std::string texturepath, LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice, CommandPool& commandpool);
	// generates mip maps of a source image
	void generateMipmaps(PhysicalDevice& physicaldevice, LogicalDevice& logicaldevice, CommandPool& commandpool);

	void createTextureImageView(LogicalDevice& logicaldevice);

	// create texture sampler used to access image texels in fragment shader
	void createTextureSampler(LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice);

	// destroys vulkan components related to Image
	virtual void destroyImage(LogicalDevice& logicaldevice);

	uint32_t mipLevels;			// number of mip levels (calculated from width and height of image)
	VkSampler vkTextureSampler;	// handle for texture sampler
	std::string fileName;

	VkImage get_vkImage() { return vkImage; }
	VkDeviceMemory get_vkImageMemory() { return vkImageMemory; }
	VkImageView get_vkImageView() { return vkImageView; }
	VkFormat get_vkFormat() { return vkFormat; }

private:
	
};


#endif
