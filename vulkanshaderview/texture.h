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

class Texture : Image {
public:
	Texture();
	~Texture();

	// creates a texture from an image
	void createTextureImage(std::string texturepath, LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice);
	// generates mip maps of a source image
	void generateMipmaps(VkFormat imageFormat, PhysicalDevice& physicaldevice);

	void createTextureImageView();

	// create texture sampler used to access image texels in fragment shader
	void createTextureSampler(LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice);

	uint32_t mipLevels;			// number of mip levels (calculated from width and height of image)
	VkSampler vkTextureSampler;	// handle for texture sampler
	std::string fileName;
private:
	
};


#endif
