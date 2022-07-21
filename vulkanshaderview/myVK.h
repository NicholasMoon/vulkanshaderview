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

#include <stb_image.h>

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

#include "debugmessenger.h"

#include "physicaldevice.h"
#include "logicaldevice.h"
#include "image.h"
#include "texture.h"
#include "depthbuffer.h"
#include "msaabuffer.h"
#include "swapchain.h"

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

#ifndef DEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    // name of validation layers to enable in debug mode
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    

private:

    GLFWwindow* window; // GLFW Window
    VkInstance instance; // Vulkan Instance
    VkDebugUtilsMessengerEXT debugMessenger; // Debug Messenger
    PhysicalDevice physicaldevice;
    LogicalDevice logicaldevice;  
    VkSurfaceKHR surface; // Window Surface
    Swapchain swapchain;
    VkRenderPass renderPass; // handle for our renderpass configuration
    VkDescriptorSetLayout descriptorSetLayout; // handle for uniform buffor object descriptor
    VkPipelineLayout pipelineLayout; // handle for uniform shader variables in pipeline
    VkPipeline graphicsPipeline; // handle for entire graphics pipeline
    std::vector<VkFramebuffer> swapChainFramebuffers; // vector of framebuffers (for each entry in swapchain)
    VkCommandPool commandPool; // handle for vulkan command pool
    std::vector<VkCommandBuffer> commandBuffers; // handle for vulkan command buffer
    std::vector<VkSemaphore> imageAvailableSemaphores; // GPU semaphore - image retrieved from swapchain and ready for rendering
    std::vector<VkSemaphore> renderFinishedSemaphores; // GPU semaphore - rendering finished and can present image
    std::vector<VkFence> inFlightFences; // CPU-GPU fence - make sure only 1 frame renders at a time
    bool framebufferResized = false; // flag that swapchain needs to be resized
    VkBuffer vertexBuffer; // handle for the vert buffer
    VkDeviceMemory vertexBufferMemory; // handle for vert memory buffer
    VkBuffer indexBuffer; // handle for the vert index buffer
    VkDeviceMemory indexBufferMemory; // handle for index memory buffer
    std::vector<VkBuffer> uniformBuffers; // array of uniform buffers (one for each frame in flight)
    std::vector<VkDeviceMemory> uniformBuffersMemory; // array of uniform buffer memory (one for each frame in flight)
    VkDescriptorPool descriptorPool; // pool of descriptor sets
    std::vector<VkDescriptorSet> descriptorSets; // array of descriptor sets
    
    
    DepthBuffer depthbuffer;
    MSAABuffer msaabuffer;

    

    std::unique_ptr<Mesh> myMesh;

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    // sets up the debug messenger handle and set debug callback parameters
    void setupDebugMessenger();

    

    

    // gets required extensions for glfw and validation callbacks (if DEBUG)
    std::vector<const char*> getRequiredExtensions();

    // check that requested validation layers are supported
    bool checkValidationLayerSupport();

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        MyVK* app = reinterpret_cast<MyVK*>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
    }

    // init glfw window
    void initWindow();

    // create vulkan instance
    void createInstance();

    // pick the GPU we will use
    void pickPhysicalDevice();


   

    // chooses swapchain surface format (color depth, color space, image resolution, etc.)
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

    // chooses swapchain presentation mode (between FIFO, relaxed, immediate, and mailbox)
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

    // chooses resolution of swap chain images (usually equal to glfw screen size)
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);


    // Create the logical device to interface with GPU
    void createLogicalDevice();

    // creates window surface
    void createSurface();

    // creates the swap chain of images
    void createSwapChain();

    // creates an image view
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

    // create image views used to interface with the images in the swapchain
    void createImageViews();

    // creates a shader module out of a shader in a bytecode array
    VkShaderModule createShaderModule(const std::vector<char>& code);

    // properties of the pipeline we wish to make dynamic (changed without recreating pipeline)
    std::vector<VkDynamicState> dynamicStates = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_LINE_WIDTH
    };

    void createGraphicsPipeline();

    void createRenderPass();

    // create framebuffers associated with entries in swapchain
    void createFramebuffers();

    // creates the vulkan command pool
    void createCommandPool();

    // creates a vulkan command pool
    void createCommandBuffers();

    // writes commands into the command buffer
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    void createSyncObjects();

    // get good memory type for buffer
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    // create a buffer and allocate memory
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

    VkCommandBuffer beginSingleTimeCommands();

    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    // copies memory from one buffer to another
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    // creates vertex buffer to store vert data
    void createVertexBuffer();

    // creates index buffer to store vert index data
    void createIndexBuffer();

    // create uniform buffer and texture sampler descriptors
    void createDescriptorSetLayout();

    // create buffers for the uniform variables (one for each frame in flight)
    void createUniformBuffers();

    // create a descriptor pool for the descriptor sets of uniform buffers and texture samplers
    void createDescriptorPool();

    void createDescriptorSets();

    // function to synchronize transition between staging buffer and vulkan image
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    // create vulkan image
    void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

    // generates mip maps of a source image
    void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

    // creates a texture from an image
    void createTextureImage();

    // creates a texture image view
    void createTextureImageView();

    // create texture sampler used to access image texels in fragment shader
    void createTextureSampler();

    // find supported image formats of the physical device
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    // find best format for depth buffer
    VkFormat findDepthFormat();

    bool hasStencilComponent(VkFormat format);

    // create vulkan structures related to depth buffer
    void createDepthResources();

    void loadModel();

    // create image resources for msaa buffer
    void createColorResources();

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