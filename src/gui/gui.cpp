#include "gui.h"

GUI::GUI() {}

GUI::~GUI() {}

// sets up ImGui support (data structure init, imgui instance init, and texture load command)
void GUI::setup(GLFWwindow* window, VulkanInstance& vulkaninstance, Surface& surface, LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice, Swapchain& swapchain, DescriptorPool& descriptorpool) {
    renderpass_imgui.createImGuiRenderPass(swapchain, physicaldevice, logicaldevice);
    commandpool_imgui.createCommandPool(physicaldevice, logicaldevice, surface);
    commandpool_imgui.createCommandBuffers(swapchain.vkSwapChainFramebuffers.size(), logicaldevice); //throws warning
    framebuffers_imgui.resize(swapchain.vkSwapChainImageViews.size());
    for (int j = 0; j < swapchain.vkSwapChainImageViews.size(); ++j) {
        framebuffers_imgui[j].createFrameBufferImGui(j, logicaldevice, swapchain, renderpass_imgui);
    }


    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = vulkaninstance.instance;
    init_info.PhysicalDevice = physicaldevice.physicalDevice;
    init_info.Device = logicaldevice.device;
    init_info.QueueFamily = physicaldevice.findQueueFamilies(surface).graphicsFamily.value();
    init_info.Queue = logicaldevice.graphicsQueue;
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = descriptorpool.vkDescriptorPool;
    init_info.Subpass = 0;
    init_info.MinImageCount = swapchain.minimumframebuffers;
    init_info.ImageCount = swapchain.vkSwapChainImageViews.size(); //throws warning
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = nullptr;
    init_info.CheckVkResultFn = check_vk_result;
    ImGui_ImplVulkan_Init(&init_info, renderpass_imgui.vkRenderPass);

    VkCommandBuffer commandbuffer_imgui = beginSingleTimeCommands(commandpool_imgui, logicaldevice);
    ImGui_ImplVulkan_CreateFontsTexture(commandbuffer_imgui);
    endSingleTimeCommands(commandbuffer_imgui, commandpool_imgui, logicaldevice);
}

// records render command for imgui to the correct command buffer
void GUI::recordRenderCommand(Swapchain& swapchain, GraphicsPipeline& graphicspipeline, uint32_t bufferIndex, uint32_t imageIndex) {
    vkResetCommandBuffer(commandpool_imgui.vkCommandBuffers[bufferIndex], 0);
    renderpass_imgui.executeImguiRenderPass(commandpool_imgui, bufferIndex, imageIndex, swapchain, graphicspipeline, framebuffers_imgui);
}

void GUI::resizeSwapchainRecreate(LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice, Swapchain& swapchain) {
    renderpass_imgui.createImGuiRenderPass(swapchain, physicaldevice, logicaldevice);
    framebuffers_imgui.resize(swapchain.vkSwapChainImageViews.size());
    for (int j = 0; j < swapchain.vkSwapChainImageViews.size(); ++j) {
        framebuffers_imgui[j].createFrameBufferImGui(j, logicaldevice, swapchain, renderpass_imgui);
    }

    ImGui_ImplVulkan_SetMinImageCount(swapchain.minimumframebuffers);
}

void GUI::resizeSwapchainDestroy(LogicalDevice& logicaldevice) {
    renderpass_imgui.destroyRenderPass(logicaldevice);

    for (int j = 0; j < framebuffers_imgui.size(); ++j) {
        framebuffers_imgui[j].destroyFrameBufferImGui(logicaldevice);
    }
}

void GUI::destroy(LogicalDevice& logicaldevice) {
    commandpool_imgui.destroyCommandPool(logicaldevice);
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GUI::updateGUI() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    //ImGui::ShowDemoWindow();
    showGUI();
    ImGui::Render();
}








IMGUI_API void GUI::showGUI(bool* p_open) {
    IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");

    // default position before .ini file created
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

    // Main body of the Demo window starts here.
    if (!ImGui::Begin("Vulkan Shaderview", p_open))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    // Leave a fixed amount of width for labels (by passing a negative value), the rest goes to widgets.
    ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

    // Menu Bar
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Menu"))
        {

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Examples"))
        {

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Tools"))
        {

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::Text("change values for the shaderview program here!");
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Lighting"))
    {
        ImGui::Text("Here is where you can change lighting!");

        ImGui::SliderFloat("light position X", &light_pos_x, -20.0f, 20.0f, "%.3f");
        ImGui::SliderFloat("light position Y", &light_pos_y, -20.0f, 20.0f, "%.3f");
        ImGui::SliderFloat("light position Z", &light_pos_z, -20.0f, 20.0f, "%.3f");

        ImGui::SliderFloat("light color R", &light_col_r, 0.0f, 100.0f, "%.3f");
        ImGui::SliderFloat("light color G", &light_col_g, 0.0f, 100.0f, "%.3f");
        ImGui::SliderFloat("light color B", &light_col_b, 0.0f, 100.0f, "%.3f");

        ImGui::SliderFloat("metallic", &metallic, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("roughness", &roughness, 0.0f, 1.0f, "%.3f");
    }

    // End of ShowDemoWindow()
    ImGui::PopItemWidth();
    ImGui::End();
}