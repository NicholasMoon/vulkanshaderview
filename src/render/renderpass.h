#ifndef RENDERPASS_H
#define RENDERPASS_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include "../utils.h"

#include "../device/physicaldevice.h"
#include "../device/logicaldevice.h"
#include "../core/swapchain.h"
#include "../image/depthbuffer.h"
#include "../image/msaabuffer.h"
#include "../vertex.h"
#include "graphicspipeline.h"
#include "../core/descriptorsets.h"
#include "../databuffer/databuffer.h"
#include "../scene/mesh.h"

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