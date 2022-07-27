#include "myVK.h"


MyVK::MyVK() : vulkaninstance(), physicaldevice(), logicaldevice(), swapchain(), 
               texture(), depthbuffer(), msaabuffer(), descriptorpool(), descriptorsets(), descriptorsetlayout(), commandpool(),
               graphicspipeline(), renderpass(), vertexbuffer(), indexbuffer()
{

}

MyVK::~MyVK() {}

void MyVK::run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

// init glfw window
void MyVK::initWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}



// creates a vulkan command pool
void MyVK::createCommandBuffers() {
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandpool.vkCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    if (vkAllocateCommandBuffers(logicaldevice.device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

// writes commands into the command buffer
void MyVK::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    // general begin recording information
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    // start render pass
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderpass.vkRenderPass;
    renderPassInfo.framebuffer = swapchain.vkSwapChainFramebuffers[imageIndex].vkFramebuffer;
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapchain.vkSwapChainExtent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicspipeline.vkGraphicsPipeline);

    VkBuffer vertexBuffers[] = { vertexbuffer.vkBuffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(commandBuffer, indexbuffer.vkBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicspipeline.vkPipelineLayout, 0, 1, &descriptorsets.vkDescriptorSets[currentFrame], 0, nullptr);

    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(myMesh->indices.size()), 1, 0, 0, 0); // vertCt, instCt, firstVert, firstInst

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void MyVK::createSyncObjects() {
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    // semaphore info
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    // fence info
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // initialize to signaled

    // create our semaphores and fences
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(logicaldevice.device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(logicaldevice.device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(logicaldevice.device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

// create buffers for the uniform variables (one for each frame in flight)
void MyVK::createUniformBuffers() {

    uniformbuffers.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        uniformbuffers[i].createUniformBuffer(logicaldevice, physicaldevice);
    }
}

void MyVK::loadModel() {
    myMesh = std::make_unique<Mesh>();

    myMesh->loadOBJ(MODEL_PATH);
}

void MyVK::pickPhysicalDevice() {
    physicaldevice.pickPhysicalDevice(vulkaninstance.instance, surface, msaabuffer.msaaSamples);
}

void MyVK::createLogicalDevice() {
    logicaldevice.createLogicalDevice(physicaldevice, surface);
}

// vulkan initialization
void MyVK::initVulkan() {
    vulkaninstance.createInstance();
    vulkaninstance.setupDebugMessenger();
    surface.createSurface(vulkaninstance, window);
    
    pickPhysicalDevice();
    
    createLogicalDevice();
    
    swapchain.createSwapChain(physicaldevice, logicaldevice, surface, window);
    swapchain.createImageViews(logicaldevice);
    
    renderpass.createRenderPass(swapchain, physicaldevice, logicaldevice, depthbuffer, msaabuffer);
    descriptorsetlayout.createDescriptorSetLayout(logicaldevice);
    graphicspipeline.createGraphicsPipeline(swapchain, logicaldevice, descriptorsetlayout, renderpass, msaabuffer);
    commandpool.createCommandPool(physicaldevice, logicaldevice, surface);
    
    msaabuffer.createColorResources(swapchain, logicaldevice, physicaldevice);
    
    depthbuffer.createDepthResources(swapchain, msaabuffer.msaaSamples, physicaldevice, logicaldevice, commandpool);
    
    swapchain.createFramebuffers(logicaldevice, renderpass, depthbuffer, msaabuffer);

    texture.createTextureImage(TEXTURE_PATH, logicaldevice, physicaldevice, commandpool);
    texture.createTextureImageView(logicaldevice);
    texture.createTextureSampler(logicaldevice, physicaldevice);

    loadModel();

    vertexbuffer.createVertexBuffer(myMesh.get(), logicaldevice, physicaldevice, commandpool);
    indexbuffer.createIndexBuffer(myMesh.get(), logicaldevice, physicaldevice, commandpool);
    createUniformBuffers();
    descriptorpool.createDescriptorPool(logicaldevice, MAX_FRAMES_IN_FLIGHT);
    descriptorsets.createDescriptorSets(MAX_FRAMES_IN_FLIGHT, logicaldevice, descriptorpool, descriptorsetlayout, uniformbuffers, texture);
    createCommandBuffers();
    createSyncObjects();
}

void MyVK::updateUniformBuffer(uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 1.0f));

    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    //std::cout << "Eye Position: " << ubo.view[0][3] << " " << ubo.view[1][3] << " " << ubo.view[2][3] << std::endl;

    ubo.proj = glm::perspective(glm::radians(45.0f), swapchain.vkSwapChainExtent.width / (float)swapchain.vkSwapChainExtent.height, 0.1f, 10.0f);

    ubo.proj[1][1] *= -1;

    ubo.modelInvTr = glm::inverse(glm::transpose(ubo.model));

    ubo.lightPos = glm::vec3(2 * sin(time) * sin(time), 2 * cos(time) * cos(time), 0.5);

    void* data;
    vkMapMemory(logicaldevice.device, uniformbuffers[currentImage].vkBufferMemory, 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(logicaldevice.device, uniformbuffers[currentImage].vkBufferMemory);
}

void MyVK::drawFrame() {
    // wait for previous frame to finish
    vkWaitForFences(logicaldevice.device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    // get image from swapchain
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(logicaldevice.device, swapchain.vkSwapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    // check if swapchain needs to be recreated
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    // only reset fence if swapchain didn't need to be recreated
    vkResetFences(logicaldevice.device, 1, &inFlightFences[currentFrame]);

    // update uniforms
    updateUniformBuffer(currentFrame);

    // record command buffer
    vkResetCommandBuffer(commandBuffers[currentFrame], 0);
    recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

    // submit command buffer
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
    VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(logicaldevice.graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    // present image to window after command buffer has finished execution
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { swapchain.vkSwapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    result = vkQueuePresentKHR(logicaldevice.presentQueue, &presentInfo);

    // check if swapchain needs to be recreated
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || swapchain.framebufferResized) {
        swapchain.framebufferResized = false;
        recreateSwapChain();
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

// handle cleanup of swapchain parts
void MyVK::cleanupSwapChain() {
    // handle msaa buffer
    vkDestroyImageView(logicaldevice.device, msaabuffer.get_vkImageView(), nullptr);
    vkDestroyImage(logicaldevice.device, msaabuffer.get_vkImage(), nullptr);
    vkFreeMemory(logicaldevice.device, msaabuffer.get_vkImageMemory(), nullptr);

    // handle depth buffers
    vkDestroyImageView(logicaldevice.device, depthbuffer.get_vkImageView(), nullptr);
    vkDestroyImage(logicaldevice.device, depthbuffer.get_vkImage(), nullptr);
    vkFreeMemory(logicaldevice.device, depthbuffer.get_vkImageMemory(), nullptr);

    // handle framebuffers
    for (size_t i = 0; i < swapchain.vkSwapChainFramebuffers.size(); i++) {
        vkDestroyFramebuffer(logicaldevice.device, swapchain.vkSwapChainFramebuffers[i].vkFramebuffer, nullptr);
    }

    // handle graphics pipeline
    vkDestroyPipeline(logicaldevice.device, graphicspipeline.vkGraphicsPipeline, nullptr);

    // handle uniform variables
    vkDestroyPipelineLayout(logicaldevice.device, graphicspipeline.vkPipelineLayout, nullptr);

    // handle render pass
    vkDestroyRenderPass(logicaldevice.device, renderpass.vkRenderPass, nullptr);

    // handle image views
    for (size_t i = 0; i < swapchain.vkSwapChainImages.size(); i++) {
        vkDestroyImageView(logicaldevice.device, swapchain.vkSwapChainImageViews[i], nullptr);
    }

    // handle swapchain
    vkDestroySwapchainKHR(logicaldevice.device, swapchain.vkSwapChain, nullptr);
}

// recreates parts of swapchain in response to trigger i.e. window resize
void MyVK::recreateSwapChain() {
    // handle minimization
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(logicaldevice.device);

    cleanupSwapChain();

    swapchain.createSwapChain(physicaldevice, logicaldevice, surface, window);
    swapchain.createImageViews(logicaldevice);
    renderpass.createRenderPass(swapchain, physicaldevice, logicaldevice, depthbuffer, msaabuffer);
    graphicspipeline.createGraphicsPipeline(swapchain, logicaldevice, descriptorsetlayout, renderpass, msaabuffer);
    msaabuffer.createColorResources(swapchain, logicaldevice, physicaldevice);
    depthbuffer.createDepthResources(swapchain, msaabuffer.msaaSamples, physicaldevice, logicaldevice, commandpool);
    swapchain.createFramebuffers(logicaldevice, renderpass, depthbuffer, msaabuffer);
}

void MyVK::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        drawFrame();
    }

    vkDeviceWaitIdle(logicaldevice.device);
}

// destroy and deallocated memory associated with glfw and vulkan 
void MyVK::cleanup() {
    // handle swapchain components
    cleanupSwapChain();

    // handle vulkan texture sampler
    vkDestroySampler(logicaldevice.device, texture.vkTextureSampler, nullptr);

    // handle vulkan texture image view
    vkDestroyImageView(logicaldevice.device, texture.get_vkImageView(), nullptr);

    // handle vulkan image used to store texture
    vkDestroyImage(logicaldevice.device, texture.get_vkImage(), nullptr);
    vkFreeMemory(logicaldevice.device, texture.get_vkImageMemory(), nullptr);

    // destroy uniform buffers and memory
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(logicaldevice.device, uniformbuffers[i].vkBuffer, nullptr);
        vkFreeMemory(logicaldevice.device, uniformbuffers[i].vkBufferMemory, nullptr);
    }

    // handle descriptor pool/sets
    vkDestroyDescriptorPool(logicaldevice.device, descriptorpool.vkDescriptorPool, nullptr);

    // destroy uniform descriptor
    vkDestroyDescriptorSetLayout(logicaldevice.device, descriptorsetlayout.vkDescriptorSetLayout, nullptr);

    // handle index buffer
    vkDestroyBuffer(logicaldevice.device, indexbuffer.vkBuffer, nullptr);

    // handle index buffer memory
    vkFreeMemory(logicaldevice.device, indexbuffer.vkBufferMemory, nullptr);

    // handle vert buffer
    vkDestroyBuffer(logicaldevice.device, vertexbuffer.vkBuffer, nullptr);

    // free vert buffer memory
    vkFreeMemory(logicaldevice.device, vertexbuffer.vkBufferMemory, nullptr);

    // handle semaphores and fences
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(logicaldevice.device, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(logicaldevice.device, imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(logicaldevice.device, inFlightFences[i], nullptr);
    }

    // handle command pool
    vkDestroyCommandPool(logicaldevice.device, commandpool.vkCommandPool, nullptr);

    // handle logical device
    vkDestroyDevice(logicaldevice.device, nullptr);

    // handle debug messenger
    if (vulkaninstance.enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(vulkaninstance.instance, vulkaninstance.debugMessenger, nullptr);
    }

    // handle window surface
    vkDestroySurfaceKHR(vulkaninstance.instance, surface.vkSurface, nullptr);

    // handle vulkan instance
    vkDestroyInstance(vulkaninstance.instance, nullptr);

    // handle glfw window
    glfwDestroyWindow(window);

    // handle glfw runtime
    glfwTerminate();
}