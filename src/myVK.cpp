#include "myVK.h"


MyVK::MyVK() : vulkaninstance(), physicaldevice(), logicaldevice(), swapchain(), 
               texture(), depthbuffer(), msaabuffer(), descriptorpool(), descriptorsets(), descriptorsetlayout(), commandpool(),
               graphicspipeline(), renderpass(), vertexbuffer(), indexbuffer() 
{}

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
    graphicspipeline.createGraphicsPipeline(swapchain, logicaldevice, descriptorsetlayout, renderpass, msaabuffer);
    commandpool.createCommandPool(physicaldevice, logicaldevice, surface);
    
    msaabuffer.createColorResources(swapchain, logicaldevice, physicaldevice);
    
    depthbuffer.createDepthResources(swapchain, msaabuffer.msaaSamples, physicaldevice, logicaldevice, commandpool);
    
    swapchain.createFramebuffers(logicaldevice, renderpass, depthbuffer, msaabuffer);

    descriptorpool.createDescriptorPool(logicaldevice, MAX_FRAMES_IN_FLIGHT);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Imgui setup begin
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Create Imgui renderpass
    renderpass_imgui.createImGuiRenderPass(swapchain, physicaldevice, logicaldevice);
    commandpool_imgui.createCommandPool(physicaldevice, logicaldevice, surface);
    commandpool_imgui.createCommandBuffers(swapchain.vkSwapChainFramebuffers.size(), logicaldevice);
    framebuffers_imgui.resize(swapchain.vkSwapChainImageViews.size());
    for (int j = 0; j < swapchain.vkSwapChainImageViews.size(); ++j) {
        framebuffers_imgui[j].createFrameBufferImGui(j, logicaldevice, swapchain, renderpass_imgui);
    }
    

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = vulkaninstance.instance;
    init_info.PhysicalDevice = physicaldevice.physicalDevice;
    init_info.Device = logicaldevice.device;
    init_info.QueueFamily = physicaldevice.findQueueFamilies(surface).graphicsFamily.value();
    init_info.Queue = logicaldevice.graphicsQueue;
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = descriptorpool.vkDescriptorPool;
    init_info.Subpass = 0;
    init_info.MinImageCount = swapchain.minimumframebuffers;
    init_info.ImageCount = swapchain.vkSwapChainImageViews.size();
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = nullptr;
    init_info.CheckVkResultFn = check_vk_result;
    ImGui_ImplVulkan_Init(&init_info, renderpass_imgui.vkRenderPass);

    VkCommandBuffer commandbuffer_imgui = beginSingleTimeCommands(commandpool_imgui, logicaldevice);
    ImGui_ImplVulkan_CreateFontsTexture(commandbuffer_imgui);
    endSingleTimeCommands(commandbuffer_imgui, commandpool_imgui, logicaldevice);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Imgui setup end
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    texture.createTextureImage(TEXTURE_PATH, logicaldevice, physicaldevice, commandpool);
    texture.createTextureImageView(logicaldevice);
    texture.createTextureSampler(logicaldevice, physicaldevice);

    loadModel();

    vertexbuffer.createVertexBuffer(myMesh.get(), logicaldevice, physicaldevice, commandpool);
    indexbuffer.createIndexBuffer(myMesh.get(), logicaldevice, physicaldevice, commandpool);
    createUniformBuffers();
    descriptorsets.createDescriptorSets(MAX_FRAMES_IN_FLIGHT, logicaldevice, descriptorpool, descriptorsetlayout, uniformbuffers, texture);
    commandpool.createCommandBuffers(MAX_FRAMES_IN_FLIGHT, logicaldevice);
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
    vkResetCommandBuffer(commandpool.vkCommandBuffers[currentFrame], 0);
    renderpass.executeRenderPass(commandpool, currentFrame, imageIndex, swapchain, graphicspipeline, descriptorsets, vertexbuffer, indexbuffer, myMesh.get());

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Imgui render code begin
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    vkResetCommandBuffer(commandpool_imgui.vkCommandBuffers[currentFrame], 0);
    renderpass_imgui.executeImguiRenderPass(commandpool_imgui, currentFrame, imageIndex, swapchain, graphicspipeline, framebuffers_imgui);

    // create a combination of normal rendering and imgui rendering command buffers
    std::array<VkCommandBuffer, 2> submitCommandBuffers =
    { commandpool.vkCommandBuffers[currentFrame], commandpool_imgui.vkCommandBuffers[currentFrame] };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Imgui render code end
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    graphicspipeline.destroyGraphicsPipeline(logicaldevice);

    // handle render pass
    renderpass.destroyRenderPass(logicaldevice);

    // imgui stuff
    renderpass_imgui.destroyRenderPass(logicaldevice);

    for (int j = 0; j < framebuffers_imgui.size(); ++j) {
        framebuffers_imgui[j].destroyFrameBufferImGui(logicaldevice);
    }

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
    graphicspipeline.createGraphicsPipeline(swapchain, logicaldevice, descriptorsetlayout, renderpass, msaabuffer);
    msaabuffer.createColorResources(swapchain, logicaldevice, physicaldevice);
    depthbuffer.createDepthResources(swapchain, msaabuffer.msaaSamples, physicaldevice, logicaldevice, commandpool);
    swapchain.createFramebuffers(logicaldevice, renderpass, depthbuffer, msaabuffer);

    renderpass_imgui.createImGuiRenderPass(swapchain, physicaldevice, logicaldevice);
    framebuffers_imgui.resize(swapchain.vkSwapChainImageViews.size());
    for (int j = 0; j < swapchain.vkSwapChainImageViews.size(); ++j) {
        framebuffers_imgui[j].createFrameBufferImGui(j, logicaldevice, swapchain, renderpass_imgui);
    }

    ImGui_ImplVulkan_SetMinImageCount(swapchain.minimumframebuffers);
}

void MyVK::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::ShowDemoWindow();
        ImGui::Render();

        drawFrame();

        
    }

    vkDeviceWaitIdle(logicaldevice.device);
}

// destroy and deallocated memory associated with glfw and vulkan 
void MyVK::cleanup() {
    // handle swapchain components
    cleanupSwapChain();

    // handle imgui
    commandpool_imgui.destroyCommandPool(logicaldevice);
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    texture.destroyImage(logicaldevice);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        uniformbuffers[i].destroyBuffer(logicaldevice);
    }
    descriptorpool.destroyDescriptorPool(logicaldevice);
    descriptorsetlayout.destroyDescriptorSetLayout(logicaldevice);
    indexbuffer.destroyBuffer(logicaldevice);
    vertexbuffer.destroyBuffer(logicaldevice);
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