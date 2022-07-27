#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Framebuffer {
public:
    Framebuffer();
    ~Framebuffer();

    VkFramebuffer vkFramebuffer; // vector of framebuffers (for each entry in swapchain)

private:

};

#endif
