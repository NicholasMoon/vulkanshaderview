#include "myVK.h"


MyVK::MyVK() : vulkaninstance(), physicaldevice(), logicaldevice(), swapchain(), 
               depthbuffer(), msaabuffer(), descriptorpool(), descriptorsetlayout(), commandpool(),
               renderpass(), m_camera()
               
{
    m_camera.m_eye = glm::vec3(0, 2.649, 34.263);
    m_camera.m_ref = glm::vec3(0, 2.472, 0);
    m_camera.m_up = glm::vec3(0, 1, 0);
    m_camera.m_fovy = glm::radians(19.5);
    m_camera.m_width = WIDTH;
    m_camera.m_height = HEIGHT;
    m_camera.recalculateAspectRatio();
    m_camera.m_near = 0.1f;
    m_camera.m_far = 80.0f;
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

// vulkan initialization
void MyVK::initVulkan() {

    vulkaninstance.createInstance();
    vulkaninstance.setupDebugMessenger();
    surface.createSurface(vulkaninstance, window);
    
    physicaldevice.pickPhysicalDevice(vulkaninstance.instance, surface, msaabuffer.msaaSamples);
    
    logicaldevice.createLogicalDevice(physicaldevice, surface);
    
    swapchain.createSwapChain(physicaldevice, logicaldevice, surface, window);
    swapchain.createImageViews(logicaldevice);

    renderpass.createRenderPass(swapchain, physicaldevice, logicaldevice, depthbuffer, msaabuffer);

    

    descriptorsetlayout.createDescriptorSetLayout(logicaldevice);

    //graphicspipeline.createGraphicsPipeline(m_shaderGGX, swapchain, logicaldevice, descriptorsetlayout, renderpass, msaabuffer);
    commandpool.createCommandPool(physicaldevice, logicaldevice, surface);
    
    msaabuffer.createColorResources(swapchain, logicaldevice, physicaldevice);
    
    
    depthbuffer.createDepthResources(swapchain, msaabuffer.msaaSamples, physicaldevice, logicaldevice, commandpool);
    
    swapchain.createFramebuffers(logicaldevice, renderpass, depthbuffer, msaabuffer);

    descriptorpool.createDescriptorPool(logicaldevice, MAX_FRAMES_IN_FLIGHT);

    m_scene.loadSceneFromJSON(m_scenefile, logicaldevice, physicaldevice, swapchain, commandpool, descriptorsetlayout, descriptorpool, renderpass, msaabuffer, MAX_FRAMES_IN_FLIGHT);

    m_gui.setup(window, vulkaninstance, surface, logicaldevice, physicaldevice, swapchain, descriptorpool, &m_scene.m_primitives, &m_camera);

    commandpool.createCommandBuffers(MAX_FRAMES_IN_FLIGHT, logicaldevice);
    createSyncObjects();
}

void MyVK::updateUniformBuffer(uint32_t currentImage) {
    m_camera.recalculateAspectRatio();

    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    for (int i = 0; i < m_scene.m_primitives.size(); ++i) {
        if (m_scene.m_primitives[i]->m_light != nullptr) {
            // light
            m_scene.m_primitives[i]->setTranslation(m_scene.m_primitives[i]->m_light->m_center);
        }
        m_scene.m_primitives[i]->m_ubo.model = m_scene.m_primitives[i]->m_modelMatrix;
        m_scene.m_primitives[i]->m_ubo.view = glm::lookAt(m_camera.m_eye, m_camera.m_ref, m_camera.m_up);
        m_scene.m_primitives[i]->m_ubo.proj = glm::perspective(m_camera.m_fovy, m_camera.m_aspect, m_camera.m_near, m_camera.m_far);
        m_scene.m_primitives[i]->m_ubo.proj[1][1] *= -1;
        m_scene.m_primitives[i]->m_ubo.modelInvTr = m_scene.m_primitives[i]->m_modelMatrixInvTrans;
        m_scene.m_primitives[i]->m_ubo.camPos = glm::vec4(m_camera.m_eye, 0);
        for (int j = 0; j < m_scene.m_lights.size(); ++j) {
            m_scene.m_primitives[i]->m_ubo.pointlights[j].lightPos = glm::vec4(m_scene.m_lights[j]->m_center, 0);
            m_scene.m_primitives[i]->m_ubo.pointlights[j].lightCol = glm::vec4(m_scene.m_lights[j]->m_intensity, 0);
        }
        

        void* data;
        vkMapMemory(logicaldevice.device, m_scene.m_primitives[i]->m_uniformbuffers[currentImage].vkBufferMemory, 0, sizeof(m_scene.m_primitives[i]->m_ubo), 0, &data);
        memcpy(data, &m_scene.m_primitives[i]->m_ubo, sizeof(m_scene.m_primitives[i]->m_ubo));
        vkUnmapMemory(logicaldevice.device, m_scene.m_primitives[i]->m_uniformbuffers[currentImage].vkBufferMemory);
    }
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
    vkResetCommandBuffer(commandpool.vkCommandBuffers[currentFrame], 0);
    renderpass.executeRenderPass(commandpool, currentFrame, imageIndex, swapchain, m_scene.m_primitives);

    // record imgui command buffer
    m_gui.recordRenderCommand(swapchain, currentFrame, imageIndex);

    // create a combination of normal rendering and imgui rendering command buffers
    std::array<VkCommandBuffer, 2> submitCommandBuffers =
    { commandpool.vkCommandBuffers[currentFrame], m_gui.commandpool_imgui.vkCommandBuffers[currentFrame] };

    // submit command buffer
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = static_cast<uint32_t>(submitCommandBuffers.size());
    submitInfo.pCommandBuffers = submitCommandBuffers.data();
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
    msaabuffer.destroyImage(logicaldevice);

    // handle depth buffers
    depthbuffer.destroyImage(logicaldevice);

    // handle graphics pipeline
    for (int i = 0; i < m_scene.m_primitives.size(); ++i) {
        m_scene.m_primitives[i]->m_material.m_pipeline.destroyGraphicsPipeline(logicaldevice);
    }

    // handle render pass
    renderpass.destroyRenderPass(logicaldevice);

    // imgui stuff
    m_gui.resizeSwapchainDestroy(logicaldevice);

    // handle swapchain (and framebuffers)
    swapchain.destroySwapChain(logicaldevice);
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
    for (int i = 0; i < m_scene.m_primitives.size(); ++i) {
        m_scene.m_primitives[i]->m_material.m_pipeline.createGraphicsPipeline(m_scene.m_primitives[i]->m_material.m_shader, swapchain, logicaldevice, descriptorsetlayout, renderpass, msaabuffer);
    }
    msaabuffer.createColorResources(swapchain, logicaldevice, physicaldevice);
    depthbuffer.createDepthResources(swapchain, msaabuffer.msaaSamples, physicaldevice, logicaldevice, commandpool);
    swapchain.createFramebuffers(logicaldevice, renderpass, depthbuffer, msaabuffer);

    // recreate/resize gui
    m_gui.resizeSwapchainRecreate(logicaldevice, physicaldevice, swapchain);

    // reset camera attributes
    m_camera.m_width = width;
    m_camera.m_height = height;
    m_camera.recalculateAspectRatio();
}

void MyVK::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        m_gui.updateGUI();

        drawFrame();

        
    }

    vkDeviceWaitIdle(logicaldevice.device);
}

// destroy and deallocated memory associated with glfw and vulkan 
void MyVK::cleanup() {
    // handle swapchain components
    cleanupSwapChain();

    // handle imgui
    m_gui.destroy(logicaldevice);

    // handle shader program
    for (int i = 0; i < m_scene.m_primitives.size(); ++i) {
        m_scene.m_primitives[i]->m_material.m_shader.destroy(logicaldevice);
    }

    // handle rest of myVK items
    for (int q = 0; q < m_scene.m_primitives.size(); ++q) {
        m_scene.m_primitives[q]->m_texture.destroyImage(logicaldevice);
        m_scene.m_primitives[q]->m_normalmap.destroyImage(logicaldevice);
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            m_scene.m_primitives[q]->m_uniformbuffers[i].destroyBuffer(logicaldevice);
        }
    }
    descriptorpool.destroyDescriptorPool(logicaldevice);
    descriptorsetlayout.destroyDescriptorSetLayout(logicaldevice);
    for (int q = 0; q < m_scene.m_primitives.size(); ++q) {
        m_scene.m_primitives[q]->m_indexbuffer.destroyBuffer(logicaldevice);
        m_scene.m_primitives[q]->m_vertexbuffer.destroyBuffer(logicaldevice);
    }
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(logicaldevice.device, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(logicaldevice.device, imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(logicaldevice.device, inFlightFences[i], nullptr);
    }
    commandpool.destroyCommandPool(logicaldevice);
    logicaldevice.destroyLogicalDevice();
    if (vulkaninstance.enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(vulkaninstance.instance, vulkaninstance.debugMessenger, nullptr);
    }
    surface.destroySurface(vulkaninstance);
    vulkaninstance.destroyInstance();
    glfwDestroyWindow(window);
    glfwTerminate();
}