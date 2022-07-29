#include "framebuffer.h"

Framebuffer::Framebuffer() {}

Framebuffer::~Framebuffer() {}


void Framebuffer::createFrameBufferImGui(uint32_t bufferIndex, LogicalDevice& logicaldevice, Swapchain& swapchain, RenderPass& renderpass) {
    std::array<VkImageView, 1> attachment = {
        swapchain.vkSwapChainImageViews[bufferIndex]
    };

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderpass.vkRenderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachment.data();
    framebufferInfo.width = swapchain.vkSwapChainExtent.width;
    framebufferInfo.height = swapchain.vkSwapChainExtent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(logicaldevice.device, &framebufferInfo, nullptr, &vkFramebuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create framebuffer!");
    }
}

void Framebuffer::destroyFrameBufferImGui(LogicalDevice& logicaldevice) {
    vkDestroyFramebuffer(logicaldevice.device, vkFramebuffer, nullptr);
}