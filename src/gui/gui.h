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
#include "../scene/primitive.h"
#include "../scene/camera/camera.h"

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
    void setup(GLFWwindow* window, VulkanInstance& vulkaninstance, Surface& surface, LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice, Swapchain& swapchain, DescriptorPool& descriptorpool, std::vector<std::unique_ptr<Primitive>>* scene_ptr, Camera* cam);
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


    std::vector<std::unique_ptr<Primitive>>*         scene_ptr_imgui;
    Camera*                                          camera_ptr_imgui;
private:

};

#endif
