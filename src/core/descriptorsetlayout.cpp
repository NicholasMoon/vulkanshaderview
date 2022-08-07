#include "descriptorsetlayout.h"

DescriptorSetLayout::DescriptorSetLayout() {}

DescriptorSetLayout::~DescriptorSetLayout() {}

// create uniform buffer and texture sampler descriptors
void DescriptorSetLayout::createDescriptorSetLayout(LogicalDevice& logicaldevice) {
    VkDescriptorSetLayoutBinding ubo_perrenderpass_LayoutBinding{};
    ubo_perrenderpass_LayoutBinding.binding = 0;
    ubo_perrenderpass_LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    ubo_perrenderpass_LayoutBinding.descriptorCount = 1;
    ubo_perrenderpass_LayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    ubo_perrenderpass_LayoutBinding.pImmutableSamplers = nullptr; // Optional

    VkDescriptorSetLayoutBinding ubo_perprimitive_LayoutBinding{};
    ubo_perprimitive_LayoutBinding.binding = 1;
    ubo_perprimitive_LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    ubo_perprimitive_LayoutBinding.descriptorCount = 1;
    ubo_perprimitive_LayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    ubo_perprimitive_LayoutBinding.pImmutableSamplers = nullptr; // Optional

    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 2;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding samplerLayoutBinding2{};
    samplerLayoutBinding2.binding = 3;
    samplerLayoutBinding2.descriptorCount = 1;
    samplerLayoutBinding2.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding2.pImmutableSamplers = nullptr;
    samplerLayoutBinding2.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 4> bindings = { ubo_perrenderpass_LayoutBinding, ubo_perprimitive_LayoutBinding, samplerLayoutBinding,  samplerLayoutBinding2 };
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(logicaldevice.device, &layoutInfo, nullptr, &vkDescriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

// create uniform buffer and texture sampler descriptors
void DescriptorSetLayout::createDescriptorSetLayout_light(LogicalDevice& logicaldevice) {
    VkDescriptorSetLayoutBinding ubo_perrenderpass_LayoutBinding{};
    ubo_perrenderpass_LayoutBinding.binding = 0;
    ubo_perrenderpass_LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    ubo_perrenderpass_LayoutBinding.descriptorCount = 1;
    ubo_perrenderpass_LayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    ubo_perrenderpass_LayoutBinding.pImmutableSamplers = nullptr; // Optional

    VkDescriptorSetLayoutBinding ubo_perprimitive_LayoutBinding{};
    ubo_perprimitive_LayoutBinding.binding = 1;
    ubo_perprimitive_LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    ubo_perprimitive_LayoutBinding.descriptorCount = 1;
    ubo_perprimitive_LayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    ubo_perprimitive_LayoutBinding.pImmutableSamplers = nullptr; // Optional

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = { ubo_perrenderpass_LayoutBinding, ubo_perprimitive_LayoutBinding };
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(logicaldevice.device, &layoutInfo, nullptr, &vkDescriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void DescriptorSetLayout::destroyDescriptorSetLayout(LogicalDevice& logicaldevice) {
    vkDestroyDescriptorSetLayout(logicaldevice.device, vkDescriptorSetLayout, nullptr);
}