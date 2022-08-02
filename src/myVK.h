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
#include "gui/gui.h"
#include "scene/material/material.h"
#include "scene/material/shaderprogram.h"
#include "scene/primitive.h"
#include "scene/camera/camera.h"
#include "scene/light/light.h"
#include "scene/light/pointlight.h"
#include "scene/light/spotlight.h"
#include "scene/light/directionallight.h"
#include "scene/light/arealight.h"

#include "scene/geometry/mesh.h"
#include "vertex.h"
#include "ubo.h"




class MyVK {

public:
    MyVK();
    ~MyVK();
    void run();

    const uint32_t WIDTH = 1280;
    const uint32_t HEIGHT = 720;
    const int MAX_FRAMES_IN_FLIGHT = 2; 
    uint32_t currentFrame = 0;

    std::string VS_PATH = "../shaders/vertshader.spv";
    std::string FS_PATH = "../shaders/pbr.spv";
    
    uint32_t incr = 0;


private:
    GLFWwindow*                     window;
    VulkanInstance                  vulkaninstance;
    Surface                         surface;
    PhysicalDevice                  physicaldevice;
    LogicalDevice                   logicaldevice;
    Swapchain                       swapchain;
    DepthBuffer                     depthbuffer;
    MSAABuffer                      msaabuffer;
    DescriptorPool                  descriptorpool;
    DescriptorSetLayout             descriptorsetlayout;
    CommandPool                     commandpool;
    GraphicsPipeline                graphicspipeline;
    RenderPass                      renderpass;

    Camera                          m_camera;
    
    GUI                             m_gui;

    ShaderProgram                   shader;

    std::vector<std::string>                     model_paths = { "../models/stanford-bunny.obj", "../models/sphere.obj" };
    std::vector<std::string>                     texture_paths = { "../textures/red.png", "../textures/white.png" };
    std::vector<std::string>                     normalmap_paths = { "../textures/norm0.png", "../textures/halfred.png" };
    std::vector<std::unique_ptr<Primitive>>      m_primitives;


      
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