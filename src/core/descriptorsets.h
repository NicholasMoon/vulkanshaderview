#ifndef DESCRIPTORSETS_H
#define DESCRIPTORSETS_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <set>
#include <iostream>
#include <vector>

#include "../device/logicaldevice.h"
#include "descriptorpool.h"
#include "descriptorsetlayout.h"

#include "../ubo.h"
#include "../image/texture.h"
#include "../image/cubemap.h"
#include "../databuffer/databuffer.h"

class Texture;
class DescriptorSetLayout;

class DescriptorSets {
public:
	DescriptorSets();
	~DescriptorSets();

	void createDescriptorSets(uint32_t maxFramesinFlight, LogicalDevice& logicaldevice, DescriptorPool& descriptorpool, DescriptorSetLayout& descriptorsetlayout, std::vector<DataBuffer>& ubufs_per_renderpass, std::vector<DataBuffer>& ubufs_per_primitive, Texture& texture, Texture& normalmap);

	void createDescriptorSets_light(uint32_t maxFramesinFlight, LogicalDevice& logicaldevice, DescriptorPool& descriptorpool, DescriptorSetLayout& descriptorsetlayout, std::vector<DataBuffer>& ubufs_per_renderpass, std::vector<DataBuffer>& ubufs_per_primitive);

	void createDescriptorSets_skybox(uint32_t maxFramesinFlight, LogicalDevice& logicaldevice, DescriptorPool& descriptorpool, DescriptorSetLayout& descriptorsetlayout, std::vector<DataBuffer>& ubufs, CubeMap& cubemap);

	std::vector<VkDescriptorSet> vkDescriptorSets;
private:

};

#endif
