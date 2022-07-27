#ifndef MYVK_H
#define MYVK_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <optional>
#include <set>
#include <cstdint> // Necessary for uint32_t
#include <limits> // Necessary for std::numeric_limits
#include <algorithm> // Necessary for std::clamp
#include <fstream>
#include <array>
#include <chrono>
#include <unordered_map>
#include <memory>

#include "utils.h"
#include "utils_vk.h"

#include "vulkaninstance.h"

#include "swapchain.h"
#include "physicaldevice.h"
#include "logicaldevice.h"
#include "image.h"
#include "texture.h"
#include "depthbuffer.h"
#include "msaabuffer.h"
#include "commandbuffer.h"
#include "graphicspipeline.h"
#include "renderpass.h"
#include "descriptorsets.h"
#include "descriptorpool.h"
#include "descriptorsetlayout.h"
#include "databuffer.h"
#include "commandpool.h"
#include "surface.h"

#include "mesh.h"
#include "vertex.h"
#include "ubo.h"


class MyVK {

public:
    MyVK();
    ~MyVK();
    void run();

    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    const int MAX_FRAMES_IN_FLIGHT = 2;
    uint32_t currentFrame = 0;

    std::string MODEL_PATH = "models/sphere.obj";
    const std::string TEXTURE_PATH = "textures/wf.png";



    

private:
    GLFWwindow*                     window;
    VulkanInstance                  vulkaninstance;
    Surface                         surface;
    PhysicalDevice                  physicaldevice;
    LogicalDevice                   logicaldevice;
    Swapchain                       swapchain;
    Texture                         texture;
    DepthBuffer                     depthbuffer;
    MSAABuffer                      msaabuffer;
    DescriptorPool                  descriptorpool;
    DescriptorSets                  descriptorsets;
    DescriptorSetLayout             descriptorsetlayout;
    CommandPool                     commandpool;
    GraphicsPipeline                graphicspipeline;
    RenderPass                      renderpass;
    DataBuffer                      vertexbuffer;
    DataBuffer                      indexbuffer;
    std::vector<DataBuffer>         uniformbuffers;


    std::unique_ptr<Mesh> myMesh;
    
    std::vector<VkCommandBuffer> commandBuffers; // handle for vulkan command buffer
    std::vector<VkSemaphore> imageAvailableSemaphores; // GPU semaphore - image retrieved from swapchain and ready for rendering
    std::vector<VkSemaphore> renderFinishedSemaphores; // GPU semaphore - rendering finished and can present image
    std::vector<VkFence> inFlightFences; // CPU-GPU fence - make sure only 1 frame renders at a time
    
    

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        MyVK* app = reinterpret_cast<MyVK*>(glfwGetWindowUserPointer(window));
        app->swapchain.framebufferResized = true;
    }

    // init glfw window
    void initWindow();

    // pick the GPU we will use
    void pickPhysicalDevice();

    // Create the logical device to interface with GPU
    void createLogicalDevice();

    // creates a vulkan command pool
    void createCommandBuffers();

    // writes commands into the command buffer
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    void createSyncObjects();

    // create buffers for the uniform variables (one for each frame in flight)
    void createUniformBuffers();

    void loadModel();

    // vulkan initialization
    void initVulkan();

    void updateUniformBuffer(uint32_t currentImage);

    void drawFrame();

    // handle cleanup of swapchain parts
    void cleanupSwapChain();

    // recreates parts of swapchain in response to trigger i.e. window resize
    void recreateSwapChain();

    void mainLoop();

    // destroy and deallocated memory associated with glfw and vulkan 
    void cleanup();
};

#endif