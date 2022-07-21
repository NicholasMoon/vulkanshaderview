#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

class Swapchain {
public:
	Swapchain();
	~Swapchain();

    void createSwapChain();
    // chooses resolution of swap chain images (usually equal to glfw screen size)
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    // chooses swapchain presentation mode (between FIFO, relaxed, immediate, and mailbox)
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    // chooses swapchain surface format (color depth, color space, image resolution, etc.)
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    void createImageViews();

    VkSwapchainKHR vkSwapChain; // Vulkan Swapchain
    std::vector<VkImage> vkSwapChainImages; // Vector of images in the swapchain
    VkFormat vkSwapChainImageFormat; // format (color depth) of images in our swapchain
    VkExtent2D vkSwapChainExtent; // extent (resolution/pixels) of images in our swapchain
    std::vector<VkImageView> vkSwapChainImageViews; // vector of image views (interface with each image in swapchain)

private:

};

#endif