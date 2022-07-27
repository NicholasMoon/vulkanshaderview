#include "descriptorpool.h"

DescriptorPool::DescriptorPool() {}

DescriptorPool::~DescriptorPool() {}

// create a descriptor pool for the descriptor sets of uniform buffers and texture samplers
void DescriptorPool::createDescriptorPool(LogicalDevice& logicaldevice, uint32_t maxFramesinFlight) {
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = maxFramesinFlight;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = maxFramesinFlight;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = maxFramesinFlight;

    if (vkCreateDescriptorPool(logicaldevice.device, &poolInfo, nullptr, &vkDescriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}