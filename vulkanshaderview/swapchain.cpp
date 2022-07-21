#include "swapchain.h"

Swapchain::Swapchain() {}

Swapchain::~Swapchain() {}

// creates the swap chain of images
void Swapchain::createSwapChain() {
    // get most suitable swap chain
    SwapChainSupportDetails swapChainSupport = physicaldevice.querySwapChainSupport(surface);

    // get color representation info
    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    // get swaping functionality
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    // get vulkan swap chain resolution/extent
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    // set number of images in swap chain equal to min + 1, check not greater than max
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    // swapchain creation info for vulkan
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1; // 1 unless stereoscopic 3D
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // rendering to screen, change to render to tex


    // handle access for multiple queue families
    QueueFamilyIndices indices = physicaldevice.findQueueFamilies(surface);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily) {
        // more than 1 queue family, so need concurrent unless we want to handle ownership explicitly
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        // only 1 queue family, so ok to have exclusive ownership
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    // rotation/scale/transform to images in the swapchain
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

    // blending windows together (not really needed)
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    // if window is occluded, just clip
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    // for recreating from a previous swapchain
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    // create swapchain with createInfo and store handle in swapChain
    if (vkCreateSwapchainKHR(logicaldevice.device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    // get images in swapchain and store them in swapChainImages vector
    vkGetSwapchainImagesKHR(logicaldevice.device, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(logicaldevice.device, swapChain, &imageCount, swapChainImages.data());

    // store swapchain format and extent
    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}

// chooses swapchain surface format (color depth, color space, image resolution, etc.)
VkSurfaceFormatKHR Swapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

// chooses swapchain presentation mode (between FIFO, relaxed, immediate, and mailbox)
VkPresentModeKHR Swapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    // try to find mailbox support (unlimited refresh rate)
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    // normal vsync swap (monitor refresh rate)
    return VK_PRESENT_MODE_FIFO_KHR;
}

// chooses resolution of swap chain images (usually equal to glfw screen size)
VkExtent2D Swapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        // use window resolution for surface extent
        return capabilities.currentExtent;
    }
    else {
        // FOR HIGH DPI DISPLAYS
        // use specify pixel extent of the vulkan swap chain based on glfw created pixel resolution/size
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

// create image views used to interface with the images in the swapchain
void Swapchain::createImageViews() {
    vkSwapChainImageViews.resize(swapChainImages.size());
    for (size_t i = 0; i < swapChainImages.size(); i++) {
        swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
    }
}