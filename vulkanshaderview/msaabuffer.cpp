#include "msaabuffer.h"

MSAABuffer::MSAABuffer() {}

MSAABuffer::~MSAABuffer() {}

// create image resources for msaa buffer
void MSAABuffer::createColorResources(Swapchain& swapchain, LogicalDevice &logicaldevice, PhysicalDevice &physicaldevice) {
    width = swapchain.vkSwapChainExtent.width;
    height = swapchain.vkSwapChainExtent.height;
    vkTiling = VK_IMAGE_TILING_OPTIMAL;
    vkUsageFlags = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    vkMemoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    vkImageAspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;

    vkFormat = swapchain.vkSwapChainImageFormat;

    createImage(1, msaaSamples, logicaldevice, physicaldevice);
    vkImageView = createImageView(1, logicaldevice);
}