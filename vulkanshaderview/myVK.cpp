#include "myVK.h"


MyVK::MyVK() : vulkaninstance(), physicaldevice(), logicaldevice(), swapchain(), texture(), depthbuffer(), msaabuffer(), graphicspipeline() {}

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



// creates window surface
void MyVK::createSurface() {
    if (glfwCreateWindowSurface(vulkaninstance.instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}





void MyVK::createRenderPass() {
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

    if (vkCreateRenderPass(logicaldevice.device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

// create framebuffers associated with entries in swapchain
void MyVK::createFramebuffers() {
    swapChainFramebuffers.resize(swapchain.vkSwapChainImages.size());

    for (size_t i = 0; i < swapchain.vkSwapChainImages.size(); i++) {
        std::array<VkImageView, 3> attachments = {
            msaabuffer.get_vkImageView(),
            depthbuffer.get_vkImageView(),
            swapchain.vkSwapChainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapchain.vkSwapChainExtent.width;
        framebufferInfo.height = swapchain.vkSwapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(logicaldevice.device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

// creates the vulkan command pool
void MyVK::createCommandPool() {
    QueueFamilyIndices queueFamilyIndices = physicaldevice.findQueueFamilies(surface);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // reset every frame
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value(); // for graphics commands

    if (vkCreateCommandPool(logicaldevice.device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}
// creates a vulkan command pool
void MyVK::createCommandBuffers() {
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
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
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapchain.vkSwapChainExtent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicspipeline.vkGraphicsPipeline);

    VkBuffer vertexBuffers[] = { vertexBuffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicspipeline.vkPipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);

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

// copies memory from one buffer to another
void MyVK::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(commandPool, logicaldevice);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer, commandPool, logicaldevice);
}

// creates vertex buffer to store vert data
void MyVK::createVertexBuffer() {
    VkDeviceSize bufferSize = sizeof(myMesh->vertices[0]) * myMesh->vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, logicaldevice, physicaldevice);

    void* data;
    vkMapMemory(logicaldevice.device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, myMesh->vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(logicaldevice.device, stagingBufferMemory);

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory, logicaldevice, physicaldevice);

    copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

    vkDestroyBuffer(logicaldevice.device, stagingBuffer, nullptr);
    vkFreeMemory(logicaldevice.device, stagingBufferMemory, nullptr);
}

// creates index buffer to store vert index data
void MyVK::createIndexBuffer() {
    VkDeviceSize bufferSize = sizeof(myMesh->indices[0]) * myMesh->indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, logicaldevice, physicaldevice);

    void* data;
    vkMapMemory(logicaldevice.device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, myMesh->indices.data(), (size_t)bufferSize);
    vkUnmapMemory(logicaldevice.device, stagingBufferMemory);

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory, logicaldevice, physicaldevice);

    copyBuffer(stagingBuffer, indexBuffer, bufferSize);

    vkDestroyBuffer(logicaldevice.device, stagingBuffer, nullptr);
    vkFreeMemory(logicaldevice.device, stagingBufferMemory, nullptr);
}

// create uniform buffer and texture sampler descriptors
void MyVK::createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(logicaldevice.device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

// create buffers for the uniform variables (one for each frame in flight)
void MyVK::createUniformBuffers() {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i], logicaldevice, physicaldevice);
    }
}

// create a descriptor pool for the descriptor sets of uniform buffers and texture samplers
void MyVK::createDescriptorPool() {
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    if (vkCreateDescriptorPool(logicaldevice.device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void MyVK::createDescriptorSets() {
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    // allocate memory for descriptor sets
    descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(logicaldevice.device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    // fill descriptor sets with info of buffers
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = texture.get_vkImageView();
        imageInfo.sampler = texture.vkTextureSampler;

        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = descriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = descriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(logicaldevice.device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
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
    createSurface();
    
    pickPhysicalDevice();
    
    createLogicalDevice();
    
    swapchain.createSwapChain(physicaldevice, logicaldevice, surface, window);
    swapchain.createImageViews(logicaldevice);
    
    createRenderPass();
    createDescriptorSetLayout();
    graphicspipeline.createGraphicsPipeline(swapchain, logicaldevice, descriptorSetLayout, renderPass, msaabuffer);
    createCommandPool();
    
    msaabuffer.createColorResources(swapchain, logicaldevice, physicaldevice);
    
    depthbuffer.createDepthResources(swapchain, msaabuffer.msaaSamples, physicaldevice, logicaldevice, commandPool);
    
    createFramebuffers();

    texture.createTextureImage(TEXTURE_PATH, logicaldevice, physicaldevice, commandPool);
    texture.createTextureImageView(logicaldevice);
    texture.createTextureSampler(logicaldevice, physicaldevice);

    loadModel();

    createVertexBuffer();
    createIndexBuffer();
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
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
    vkMapMemory(logicaldevice.device, uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(logicaldevice.device, uniformBuffersMemory[currentImage]);
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
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        framebufferResized = false;
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
    for (size_t i = 0; i < swapChainFramebuffers.size(); i++) {
        vkDestroyFramebuffer(logicaldevice.device, swapChainFramebuffers[i], nullptr);
    }

    // handle graphics pipeline
    vkDestroyPipeline(logicaldevice.device, graphicspipeline.vkGraphicsPipeline, nullptr);

    // handle uniform variables
    vkDestroyPipelineLayout(logicaldevice.device, graphicspipeline.vkPipelineLayout, nullptr);

    // handle render pass
    vkDestroyRenderPass(logicaldevice.device, renderPass, nullptr);

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
    createRenderPass();
    graphicspipeline.createGraphicsPipeline(swapchain, logicaldevice, descriptorSetLayout, renderPass, msaabuffer);
    msaabuffer.createColorResources(swapchain, logicaldevice, physicaldevice);
    depthbuffer.createDepthResources(swapchain, msaabuffer.msaaSamples, physicaldevice, logicaldevice, commandPool);
    createFramebuffers();
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
        vkDestroyBuffer(logicaldevice.device, uniformBuffers[i], nullptr);
        vkFreeMemory(logicaldevice.device, uniformBuffersMemory[i], nullptr);
    }

    // handle descriptor pool/sets
    vkDestroyDescriptorPool(logicaldevice.device, descriptorPool, nullptr);

    // destroy uniform descriptor
    vkDestroyDescriptorSetLayout(logicaldevice.device, descriptorSetLayout, nullptr);

    // handle index buffer
    vkDestroyBuffer(logicaldevice.device, indexBuffer, nullptr);

    // handle index buffer memory
    vkFreeMemory(logicaldevice.device, indexBufferMemory, nullptr);

    // handle vert buffer
    vkDestroyBuffer(logicaldevice.device, vertexBuffer, nullptr);

    // free vert buffer memory
    vkFreeMemory(logicaldevice.device, vertexBufferMemory, nullptr);

    // handle semaphores and fences
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(logicaldevice.device, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(logicaldevice.device, imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(logicaldevice.device, inFlightFences[i], nullptr);
    }

    // handle command pool
    vkDestroyCommandPool(logicaldevice.device, commandPool, nullptr);

    // handle logical device
    vkDestroyDevice(logicaldevice.device, nullptr);

    // handle debug messenger
    if (vulkaninstance.enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(vulkaninstance.instance, vulkaninstance.debugMessenger, nullptr);
    }

    // handle window surface
    vkDestroySurfaceKHR(vulkaninstance.instance, surface, nullptr);

    // handle vulkan instance
    vkDestroyInstance(vulkaninstance.instance, nullptr);

    // handle glfw window
    glfwDestroyWindow(window);

    // handle glfw runtime
    glfwTerminate();
}