#include "descriptorsets.h"

DescriptorSets::DescriptorSets() {}

DescriptorSets::~DescriptorSets() {}

void DescriptorSets::createDescriptorSets(uint32_t maxFramesinFlight, LogicalDevice& logicaldevice, DescriptorPool& descriptorpool, DescriptorSetLayout &descriptorsetlayout, std::vector<DataBuffer> &uniformBuffers, Texture& texture) {
    std::vector<VkDescriptorSetLayout> layouts(maxFramesinFlight, descriptorsetlayout.vkDescriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorpool.vkDescriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(maxFramesinFlight);
    allocInfo.pSetLayouts = layouts.data();

    // allocate memory for descriptor sets
    vkDescriptorSets.resize(maxFramesinFlight);
    if (vkAllocateDescriptorSets(logicaldevice.device, &allocInfo, vkDescriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    // fill descriptor sets with info of buffers
    for (size_t i = 0; i < maxFramesinFlight; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i].vkBuffer;
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = texture.get_vkImageView();
        imageInfo.sampler = texture.vkTextureSampler;

        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = vkDescriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = vkDescriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(logicaldevice.device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}