#ifndef PHYSICALDEVICE_H
#define PHYSICALDEVICE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <set>
#include <iostream>
#include <vector>

#include "utils_vk.h"

class PhysicalDevice {
public:
	PhysicalDevice();
	~PhysicalDevice();

    // tests whether passed in GPU has the extensions we need
    bool checkDeviceExtensionSupport();

    // Finds a graphics queue family (given a GPU)
    QueueFamilyIndices findQueueFamilies(VkSurfaceKHR& surface);

    SwapChainSupportDetails querySwapChainSupport(VkSurfaceKHR& surface);

    // tests whether passed in GPU has the extensions/queue families we need
    bool isDeviceSuitable(VkSurfaceKHR& surface);

    // get max msaa sample count supported by physical device
    VkSampleCountFlagBits getMaxUsableSampleCount();

    // pick the GPU we will use
    void pickPhysicalDevice(VkInstance& instance, VkSurfaceKHR& surface, VkSampleCountFlagBits &msaaSamples);

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE; // GPU

    // name of device extensions to enable (swapchain)
    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

private:

};
#endif
