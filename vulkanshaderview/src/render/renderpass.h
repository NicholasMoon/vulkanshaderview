#ifndef RENDERPASS_H
#define RENDERPASS_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include "utils.h"

#include "physicaldevice.h"
#include "logicaldevice.h"
#include "swapchain.h"
#include "depthbuffer.h"
#include "msaabuffer.h"
#include "vertex.h"
#include "graphicspipeline.h"
#include "descriptorsets.h"
#include "databuffer.h"
#include "mesh.h"

class Swapchain;
class MSAABuffer;
class DepthBuffer;
class GraphicsPipeline;

class RenderPass {
public:
	RenderPass();
	~RenderPass();

	void createRenderPass(Swapchain& swapchain, PhysicalDevice& physicaldevice, LogicalDevice& logicaldevice, DepthBuffer& depthbuffer, MSAABuffer& msaabuffer);

	void executeRenderPass(CommandPool& commandpool, uint32_t bufferIndex, uint32_t imageIndex, Swapchain& swapchain, GraphicsPipeline& graphicspipeline, DescriptorSets& descriptorsets, DataBuffer& vertexbuffer, DataBuffer& indexbuffer, Mesh *myMesh);

	void destroyRenderPass(LogicalDevice& logicaldevice);

	VkRenderPass vkRenderPass; // handle for our renderpass configuration

private:

};

#endif