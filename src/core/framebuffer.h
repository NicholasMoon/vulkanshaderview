#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "../device/logicaldevice.h"
#include "swapchain.h"
#include "../render/renderpass.h"

class RenderPass;

class Framebuffer {
public:
    Framebuffer();
    ~Framebuffer();

    void createFrameBufferImGui(uint32_t bufferIndex, LogicalDevice& logicaldevice, Swapchain& swapchain, RenderPass& renderpass);

    void destroyFrameBufferImGui(LogicalDevice& logicaldevice);

    VkFramebuffer vkFramebuffer; // vector of framebuffers (for each entry in swapchain)

private:

};

#endif
