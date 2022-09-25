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
}

void MSAABuffer::destroyImage(LogicalDevice& logicaldevice) {
    vkDestroyImageView(logicaldevice.device, vkImageView, nullptr);
    vkDestroyImage(logicaldevice.device, vkImage, nullptr);
    vkFreeMemory(logicaldevice.device, vkImageMemory, nullptr);
}