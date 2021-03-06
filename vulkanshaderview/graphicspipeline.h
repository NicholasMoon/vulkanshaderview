#ifndef GRAPHICSPIPELINE_H
#define GRAPHICSPIPELINE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include "utils.h"

#include "physicaldevice.h"
#include "logicaldevice.h"
#include "swapchain.h"
#include "msaabuffer.h"
#include "vertex.h"

class GraphicsPipeline {
public:
	GraphicsPipeline();
	~GraphicsPipeline();

	void createGraphicsPipeline(Swapchain& swapchain, LogicalDevice& logicaldevice, VkDescriptorSetLayout& descriptorSetLayout, VkRenderPass& renderPass, MSAABuffer& msaabuffer);
	
	VkPipeline vkGraphicsPipeline; // handle for entire graphics pipeline
	VkPipelineLayout vkPipelineLayout; // handle for uniform shader variables in pipeline

	// properties of the pipeline we wish to make dynamic (changed without recreating pipeline)
	std::vector<VkDynamicState> dynamicStates = {
	VK_DYNAMIC_STATE_VIEWPORT,
	VK_DYNAMIC_STATE_LINE_WIDTH
	};

private:

};

// creates a shader module out of a shader in a bytecode array
VkShaderModule createShaderModule(const std::vector<char>& code, LogicalDevice& logicaldevice);

#endif

