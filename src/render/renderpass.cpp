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

    // resolve msaa buffer to presentable swapchain image EDIT FOR IMGUI: Set to color attachment optimal for imgui postprocess rendering stage
    VkAttachmentDescription colorAttachmentResolve{};
    colorAttachmentResolve.format = swapchain.vkSwapChainImageFormat;
    colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // PRESENT_SRC_KHR changed to COLOR_ATTACHMENT_OPTIMAL
    //colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

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
void RenderPass::executeRenderPass(CommandPool& commandpool, uint32_t bufferIndex, uint32_t imageIndex, Swapchain& swapchain, std::vector<std::unique_ptr<Primitive>>& primitives) {
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

    

    for (int meshIndex = 0; meshIndex < primitives.size(); ++meshIndex) {
        vkCmdBindPipeline(commandpool.vkCommandBuffers[bufferIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, primitives[meshIndex]->m_material->m_pipeline.vkGraphicsPipeline);

        VkBuffer vertexBuffers[] = { primitives[meshIndex]->m_vertexbuffer.vkBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandpool.vkCommandBuffers[bufferIndex], 0, 1, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(commandpool.vkCommandBuffers[bufferIndex], primitives[meshIndex]->m_indexbuffer.vkBuffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(commandpool.vkCommandBuffers[bufferIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, primitives[meshIndex]->m_material->m_pipeline.vkPipelineLayout, 0, 1, &primitives[meshIndex]->m_descriptorsets.vkDescriptorSets[bufferIndex], 0, nullptr);

        vkCmdDrawIndexed(commandpool.vkCommandBuffers[bufferIndex], static_cast<uint32_t>(primitives[meshIndex]->m_geometry->indices.size()), 1, 0, 0, 0); // vertCt, instCt, firstVert, firstInst
    }



    vkCmdEndRenderPass(commandpool.vkCommandBuffers[bufferIndex]);

    // end recording command buffer
    commandpool.endRecordCommandBuffer(bufferIndex);
}

// render pass specifically for ImGui
void RenderPass::createImGuiRenderPass(Swapchain& swapchain, PhysicalDevice& physicaldevice, LogicalDevice& logicaldevice) {
    // color attachment info
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapchain.vkSwapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD; // draw over main rendering
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // reference to a color attachment
    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // render subpass options
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    // subpass dependencies
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    // create render pass
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(logicaldevice.device, &renderPassInfo, nullptr, &vkRenderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create ImGui render pass!");
    }
}

void RenderPass::executeImguiRenderPass(CommandPool& commandpool, uint32_t bufferIndex, uint32_t imageIndex, Swapchain& swapchain, std::vector<Framebuffer>& framebuffers) {
    // start recording command buffer
    commandpool.startRecordCommandBuffer(bufferIndex);
    
    std::array<VkClearValue, 1> clearValues{};
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    
    VkRenderPassBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.renderPass = vkRenderPass;
    info.framebuffer = framebuffers[imageIndex].vkFramebuffer;
    info.renderArea.offset = { 0, 0 };
    info.renderArea.extent = swapchain.vkSwapChainExtent;
    info.clearValueCount = static_cast<uint32_t>(clearValues.size());
    info.pClearValues = clearValues.data();
    vkCmdBeginRenderPass(commandpool.vkCommandBuffers[bufferIndex], &info, VK_SUBPASS_CONTENTS_INLINE);

    // Record Imgui Draw Data and draw funcs into command buffer
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandpool.vkCommandBuffers[bufferIndex]);

    vkCmdEndRenderPass(commandpool.vkCommandBuffers[bufferIndex]);

    commandpool.endRecordCommandBuffer(bufferIndex);
}

void RenderPass::destroyRenderPass(LogicalDevice& logicaldevice) {
    vkDestroyRenderPass(logicaldevice.device, vkRenderPass, nullptr);
}