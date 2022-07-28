#include "surface.h"

Surface::Surface() {}

Surface::~Surface() {}

// creates window surface
void Surface::createSurface(VulkanInstance& vulkaninstance, GLFWwindow* window) {
    if (glfwCreateWindowSurface(vulkaninstance.instance, window, nullptr, &vkSurface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

void Surface::destroySurface(VulkanInstance& vulkaninstance) {
    vkDestroySurfaceKHR(vulkaninstance.instance, vkSurface, nullptr);
}