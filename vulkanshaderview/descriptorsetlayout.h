#ifndef DESCRIPTORSETLAYOUT_H
#define DESCRIPTORSETLAYOUT_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <set>
#include <iostream>
#include <vector>

#include "logicaldevice.h"
#include "descriptorpool.h"

#include "ubo.h"
#include "texture.h"

class DescriptorSetLayout {
public:
	DescriptorSetLayout();
	~DescriptorSetLayout();

	// create uniform buffer and texture sampler descriptors
	void createDescriptorSetLayout(LogicalDevice& logicaldevice);

	void destroyDescriptorSetLayout(LogicalDevice& logicaldevice);

	VkDescriptorSetLayout vkDescriptorSetLayout;
private:

};

#endif
