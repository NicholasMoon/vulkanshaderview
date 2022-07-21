#ifndef MSAABUFFER_H
#define MSAABUFFER_H

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

class MSAABuffer : Image {
public:
	MSAABuffer();
	~MSAABuffer();



	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT; // max supported msaa samples

private:

};

#endif