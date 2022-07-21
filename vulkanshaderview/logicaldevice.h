#ifndef LOGICALDEVICE_H
#define LOGICALDEVICE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <set>
#include <iostream>
#include <vector>

#include "utils_vk.h"
#include "physicaldevice.h"

class LogicalDevice {
public:

    LogicalDevice();
    ~LogicalDevice();


    // Create the logical device to interface with GPU
    void createLogicalDevice(PhysicalDevice& physicaldevice, VkSurfaceKHR& surface);

    VkDevice device; // Logical Device
    VkQueue graphicsQueue; // Graphics Queue
    VkQueue presentQueue; // Window Surface Queue

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

};

#endif
