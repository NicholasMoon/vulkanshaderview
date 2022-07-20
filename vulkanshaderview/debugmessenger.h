#ifndef DEBUGMESSENGER_H
#define DEBUGMESSENGER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// function to get and enable extension required to utilize the debug messenger and callbacks
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger);

// function to disable extension required to utilize the debug messenger and callbacks
void DestroyDebugUtilsMessengerEXT(VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator);

#endif