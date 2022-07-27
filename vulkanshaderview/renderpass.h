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

class Swapchain;
class MSAABuffer;
class DepthBuffer;

class RenderPass {
public:
	RenderPass();
	~RenderPass();

	void createRenderPass(Swapchain& swapchain, PhysicalDevice& physicaldevice, LogicalDevice& logicaldevice, DepthBuffer& depthbuffer, MSAABuffer& msaabuffer);

	VkRenderPass vkRenderPass; // handle for our renderpass configuration



private:

};

#endif