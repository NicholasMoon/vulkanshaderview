#ifndef GUI_H
#define GUI_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "../core/commandpool.h"
#include "../render/renderpass.h"
#include "../core/framebuffer.h"
#include "../core/descriptorpool.h"
#include "../device/logicaldevice.h"
#include "../device/physicaldevice.h"
#include "../core/vulkaninstance.h"
#include "../core/surface.h"
#include "../core/swapchain.h"

static void check_vk_result(VkResult err)
{
    if (err == 0)
        return;
    fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
    if (err < 0)
        abort();
}

class GUI {
public:
    GUI();
    ~GUI();

    // app interface functions
    void setup(GLFWwindow* window, VulkanInstance& vulkaninstance, Surface& surface, LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice, Swapchain& swapchain, DescriptorPool& descriptorpool);
    void recordRenderCommand(Swapchain& swapchain, uint32_t bufferIndex, uint32_t imageIndex);
    void updateGUI();
    void resizeSwapchainRecreate(LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice, Swapchain& swapchain);
    void resizeSwapchainDestroy(LogicalDevice& logicaldevice);
    void destroy(LogicalDevice& logicaldevice);

    // actual imgui window code
    IMGUI_API void showGUI(bool* p_open = NULL);

    CommandPool                     commandpool_imgui;
    RenderPass                      renderpass_imgui;
    std::vector<Framebuffer>        framebuffers_imgui;

    float light_pos_x = 0.0f, light_pos_y = 1.0f, light_pos_z = -1.5f;
    float light_col_r = 10.0f, light_col_g = 10.0f, light_col_b = 10.0f;

    float cam_eye_x = 0.0f, cam_eye_y = 3.75f, cam_eye_z = 7.5f;
    float cam_ref_x = 0.0f, cam_ref_y = 0.0f, cam_ref_z = 0.0f;

    float metallic = 0.5f, roughness = 0.5f;
private:

};

#endif
