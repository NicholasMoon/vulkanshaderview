#include "cubemap.h"

CubeMap::CubeMap() {}

CubeMap::~CubeMap() {}

// creates a texture from an image
void CubeMap::createCubeMapImage(std::string directorypath, LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice, CommandPool& commandpool) {
    directoryPath = directorypath;

    int texWidth, texHeight, texChannels;
    stbi_uc* pixels[6];
    pixels[0] = stbi_load((directoryPath + "/posz.jpg").c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    pixels[1] = stbi_load((directoryPath + "/negz.jpg").c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    pixels[2] = stbi_load((directoryPath + "/posy.jpg").c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    pixels[3] = stbi_load((directoryPath + "/negy.jpg").c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    pixels[4] = stbi_load((directoryPath + "/posx.jpg").c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    pixels[5] = stbi_load((directoryPath + "/negx.jpg").c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4 * 6;
    VkDeviceSize layerSize = imageSize / 6;
    
    
    width = texWidth;
    height = texHeight;
    channels = texChannels;
    vkTiling = VK_IMAGE_TILING_OPTIMAL;
    vkUsageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    vkMemoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    vkImageAspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
    vkFormatFeatureFlags = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    vkFormat = VK_FORMAT_R8G8B8A8_SRGB;

    if (!pixels) {
        throw std::runtime_error("failed to load cubemap image!");
    }

    mipLevels = 1;

    // staging buffer for image data
    DataBuffer stagingBuffer(imageSize);
    stagingBuffer.createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, logicaldevice, physicaldevice);



    // transfer data to staging buffer
    stbi_uc* data;
    vkMapMemory(logicaldevice.device, stagingBuffer.vkBufferMemory, 0, imageSize, 0, (void**) &data);


    for (uint8_t i = 0; i < 6; ++i) {
        memcpy(data + (layerSize * i), pixels[i], static_cast<size_t>(layerSize));
    }
    
    vkUnmapMemory(logicaldevice.device, stagingBuffer.vkBufferMemory);

    for (int i = 0; i < 6; ++i) {
        stbi_image_free(pixels[i]);
    }
    

    // vulkan image creation struct
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 6;
    imageInfo.format = vkFormat;
    imageInfo.tiling = vkTiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = vkUsageFlags;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT; // for msaa
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    createImage(1, imageInfo, logicaldevice, physicaldevice);

    // transition the texture image to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
    transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, commandpool, logicaldevice, 6);

    // execute the buffer to image copy operation
    copyBufferToImage(stagingBuffer.vkBuffer, commandpool, logicaldevice, 6);

    transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, commandpool, logicaldevice, 6);

    //transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps
    //generateMipmaps(physicaldevice, logicaldevice, commandpool);

    vkDestroyBuffer(logicaldevice.device, stagingBuffer.vkBuffer, nullptr);
    vkFreeMemory(logicaldevice.device, stagingBuffer.vkBufferMemory, nullptr);
}

// creates a texture image view
void CubeMap::createCubeMapImageView(LogicalDevice &logicaldevice) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = vkImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
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
    viewInfo.subresourceRange.layerCount = 6;

    if (vkCreateImageView(logicaldevice.device, &viewInfo, nullptr, &vkImageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }
}

// create texture sampler used to access image texels in fragment shader
void CubeMap::createCubeMapSampler(LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice) {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(physicaldevice.physicalDevice, &properties);
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    if (vkCreateSampler(logicaldevice.device, &samplerInfo, nullptr, &vkTextureSampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

void CubeMap::destroyImage(LogicalDevice& logicaldevice) {
    vkDestroySampler(logicaldevice.device, vkTextureSampler, nullptr);
    vkDestroyImageView(logicaldevice.device, vkImageView, nullptr);
    vkDestroyImage(logicaldevice.device, vkImage, nullptr);
    vkFreeMemory(logicaldevice.device, vkImageMemory, nullptr);
}