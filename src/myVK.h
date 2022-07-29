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

#include "imgui.h"
#include "imgui_internal.h"
#include "imstb_rectpack.h"
#include "imstb_textedit.h"
#include "imstb_truetype.h"
#include "imconfig.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

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

#include "core/vulkaninstance.h"

#include "core/swapchain.h"
#include "device/physicaldevice.h"
#include "device/logicaldevice.h"
#include "image/image.h"
#include "image/texture.h"
#include "image/depthbuffer.h"
#include "image/msaabuffer.h"
#include "commandbuffer.h"
#include "render/graphicspipeline.h"
#include "render/renderpass.h"
#include "core/descriptorsets.h"
#include "core/descriptorpool.h"
#include "core/descriptorsetlayout.h"
#include "databuffer/databuffer.h"
#include "core/commandpool.h"
#include "core/surface.h"

#include "scene/mesh.h"
#include "vertex.h"
#include "ubo.h"

static void check_vk_result(VkResult err)
{
    if (err == 0)
        return;
    fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
    if (err < 0)
        abort();
}


class MyVK {

public:
    MyVK();
    ~MyVK();
    void run();

    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    const int MAX_FRAMES_IN_FLIGHT = 2; 
    uint32_t currentFrame = 0;

    std::string MODEL_PATH = "../models/sphere.obj";
    const std::string TEXTURE_PATH = "../textures/wf.png";

    

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

    CommandPool                     commandpool_imgui;
    RenderPass                      renderpass_imgui;
    std::vector<Framebuffer>        framebuffers_imgui;


    std::unique_ptr<Mesh> myMesh;
      
    std::vector<VkSemaphore> imageAvailableSemaphores; // GPU semaphore - image retrieved from swapchain and ready for rendering
    std::vector<VkSemaphore> renderFinishedSemaphores; // GPU semaphore - rendering finished and can present image
    std::vector<VkFence> inFlightFences; // CPU-GPU fence - make sure only 1 frame renders at a time
    
    

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        MyVK* app = reinterpret_cast<MyVK*>(glfwGetWindowUserPointer(window));
        app->swapchain.framebufferResized = true;
    }

    // init glfw window
    void initWindow();

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