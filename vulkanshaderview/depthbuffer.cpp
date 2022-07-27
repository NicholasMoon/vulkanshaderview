#include "depthbuffer.h"

DepthBuffer::DepthBuffer() {}

DepthBuffer::~DepthBuffer() {}

// create vulkan structures related to depth buffer
void DepthBuffer::createDepthResources(Swapchain &swapchain, VkSampleCountFlagBits msaaSamples, PhysicalDevice& physicaldevice, LogicalDevice &logicaldevice, CommandPool& commandpool) {
    width = swapchain.vkSwapChainExtent.width;
    height = swapchain.vkSwapChainExtent.height;
    vkTiling = VK_IMAGE_TILING_OPTIMAL;
    vkUsageFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    vkMemoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    vkImageAspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
    vkFormatFeatureFlags = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

    vkFormat = findDepthFormat(physicaldevice);

    createImage(1, msaaSamples, logicaldevice, physicaldevice);
    vkImageView = createImageView(1, logicaldevice);

    transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1, commandpool, logicaldevice);
}

// find best format for depth buffer
VkFormat DepthBuffer::findDepthFormat(PhysicalDevice& physicaldevice) {
    return findSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, physicaldevice
    );
}

// find supported image formats of the physical device
VkFormat DepthBuffer::findSupportedFormat(const std::vector<VkFormat>& candidates, PhysicalDevice &physicaldevice) {
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicaldevice.physicalDevice, format, &props);

        if (vkTiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & vkFormatFeatureFlags) == vkFormatFeatureFlags) {
            return format;
        }
        else if (vkTiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & vkFormatFeatureFlags) == vkFormatFeatureFlags) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}