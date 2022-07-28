#include "renderpass.h"

RenderPass::RenderPass() {}

RenderPass::~RenderPass() {}

void RenderPass::createRenderPass(Swapchain &swapchain, PhysicalDevice &physicaldevice, LogicalDevice &logicaldevice, DepthBuffer &depthbuffer, MSAABuffer &msaabuffer) {
    // color attachment info
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapchain.vkSwapChainImageFormat;
    colorAttachment.samples = msaabuffer.msaaSamples; // for msaa
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // cant present msaa directly

    // depth buffer attachment info
    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = depthbuffer.findDepthFormat(physicaldevice);
    depthAttachment.samples = msaabuffer.msaaSamples; // for msaa
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // reference to a color attachment
    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // reference to depth buffer attachment
    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // resolve msaa buffer to presentable swapchain image
    VkAttachmentDescription colorAttachmentResolve{};
    colorAttachmentResolve.format = swapchain.vkSwapChainImageFormat;
    colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // reference to msaa->swapchain resolve attachment
    VkAttachmentReference colorAttachmentResolveRef{};
    colorAttachmentResolveRef.attachment = 2;
    colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // render subpass options
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;
    subpass.pResolveAttachments = &colorAttachmentResolveRef;

    std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };

    // subpass dependencies
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcAccessMask = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    // create render pass
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(logicaldevice.device, &renderPassInfo, nullptr, &vkRenderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

// execute render pass and record command buffer
void RenderPass::executeRenderPass(CommandPool& commandpool, uint32_t bufferIndex, uint32_t imageIndex, Swapchain& swapchain, GraphicsPipeline& graphicspipeline, DescriptorSets& descriptorsets, DataBuffer& vertexbuffer, DataBuffer& indexbuffer, Mesh *myMesh) {
    // start recording command buffer
    commandpool.startRecordCommandBuffer(bufferIndex);

    // start render pass
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = vkRenderPass;
    renderPassInfo.framebuffer = swapchain.vkSwapChainFramebuffers[imageIndex].vkFramebuffer;
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapchain.vkSwapChainExtent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandpool.vkCommandBuffers[bufferIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandpool.vkCommandBuffers[bufferIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicspipeline.vkGraphicsPipeline);

    VkBuffer vertexBuffers[] = { vertexbuffer.vkBuffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandpool.vkCommandBuffers[bufferIndex], 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(commandpool.vkCommandBuffers[bufferIndex], indexbuffer.vkBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(commandpool.vkCommandBuffers[bufferIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicspipeline.vkPipelineLayout, 0, 1, &descriptorsets.vkDescriptorSets[bufferIndex], 0, nullptr);

    vkCmdDrawIndexed(commandpool.vkCommandBuffers[bufferIndex], static_cast<uint32_t>(myMesh->indices.size()), 1, 0, 0, 0); // vertCt, instCt, firstVert, firstInst

    vkCmdEndRenderPass(commandpool.vkCommandBuffers[bufferIndex]);

    // end recording command buffer
    commandpool.endRecordCommandBuffer(bufferIndex);
}

void RenderPass::destroyRenderPass(LogicalDevice& logicaldevice) {
    vkDestroyRenderPass(logicaldevice.device, vkRenderPass, nullptr);
}