#ifndef UTILS_VK_H
#define UTILS_VK_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <optional>
#include <vector>



// struct that holds list of queue families
struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily; // queue family that supports graphics operations
    std::optional<uint32_t> presentFamily; // queue family that supports window presentation operations

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};



#endif
