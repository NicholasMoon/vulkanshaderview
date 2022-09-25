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

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = vkFormat;
    imageInfo.tiling = vkTiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = vkUsageFlags;
    imageInfo.samples = msaaSamples; // for msaa
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    createImage(1, imageInfo, logicaldevice, physicaldevice);

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = vkImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = vkFormat;
    // used for swizzling components of image
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    // image used as a color target or depth target etc
    viewInfo.subresourceRange.aspectMask = vkImageAspectFlags;
    // mip mapping
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    // single layer per image (multiple for stereoscopic 3D)
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
    vkImageView = createImageView(viewInfo, logicaldevice);

    transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1, commandpool, logicaldevice, 1);
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

void DepthBuffer::destroyImage(LogicalDevice& logicaldevice) {
    vkDestroyImageView(logicaldevice.device, vkImageView, nullptr);
    vkDestroyImage(logicaldevice.device, vkImage, nullptr);
    vkFreeMemory(logicaldevice.device, vkImageMemory, nullptr);
}