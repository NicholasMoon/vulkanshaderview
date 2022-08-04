#ifndef RENDERPASS_H
#define RENDERPASS_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_internal.h"
#include "imstb_rectpack.h"
#include "imstb_textedit.h"
#include "imstb_truetype.h"
#include "imconfig.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

#include <iostream>
#include <vector>
#include <memory>

#include "../utils.h"

#include "../device/physicaldevice.h"
#include "../device/logicaldevice.h"
#include "../core/swapchain.h"
#include "../core/framebuffer.h"
#include "../image/depthbuffer.h"
#include "../image/msaabuffer.h"
#include "../core/descriptorsets.h"
#include "../databuffer/databuffer.h"
#include "../scene/primitive.h"

class Swapchain;
class MSAABuffer;
class DepthBuffer;

class Framebuffer;
class Primitive;

class RenderPass {
public:
	RenderPass();
	~RenderPass();

	void createRenderPass(Swapchain& swapchain, PhysicalDevice& physicaldevice, LogicalDevice& logicaldevice, DepthBuffer& depthbuffer, MSAABuffer& msaabuffer);

	void executeRenderPass(CommandPool& commandpool, uint32_t bufferIndex, uint32_t imageIndex, Swapchain& swapchain, std::vector<std::unique_ptr<Primitive>>& primitive);

	void createImGuiRenderPass(Swapchain& swapchain, PhysicalDevice& physicaldevice, LogicalDevice& logicaldevice);

	void executeImguiRenderPass(CommandPool& commandpool, uint32_t bufferIndex, uint32_t imageIndex, Swapchain& swapchain, std::vector<Framebuffer>& framebuffers);

	void destroyRenderPass(LogicalDevice& logicaldevice);

	VkRenderPass vkRenderPass; // handle for our renderpass configuration

private:

};

#endif