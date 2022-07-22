#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <algorithm>

#include "image.h"

#include "physicaldevice.h"
#include "logicaldevice.h"

class Swapchain {
public:
	Swapchain();
	~Swapchain();

    void createSwapChain(PhysicalDevice& physicaldevice, LogicalDevice& logicaldevice, VkSurfaceKHR& surface, GLFWwindow* window);
    // chooses resolution of swap chain images (usually equal to glfw screen size)
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
    // chooses swapchain presentation mode (between FIFO, relaxed, immediate, and mailbox)
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    // chooses swapchain surface format (color depth, color space, image resolution, etc.)
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    void createImageViews(LogicalDevice& logicaldevice);

    VkSwapchainKHR vkSwapChain; // Vulkan Swapchain
    std::vector<VkImage> vkSwapChainImages; // Vector of images in the swapchain
    std::vector<VkImageView> vkSwapChainImageViews; // Vector of imageviews in the swapchain
    VkFormat vkSwapChainImageFormat; // format (color depth) of images in our swapchain
    VkExtent2D vkSwapChainExtent; // extent (resolution/pixels) of images in our swapchain
    VkImageAspectFlags vkSwapChainAspectFlags; // aspect flags for swapchain

private:

};

#endif