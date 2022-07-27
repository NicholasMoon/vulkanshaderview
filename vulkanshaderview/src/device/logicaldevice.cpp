#include "logicaldevice.h"

LogicalDevice::LogicalDevice() {}

LogicalDevice::~LogicalDevice() {}

// Create the logical device to interface with GPU
void LogicalDevice::createLogicalDevice(PhysicalDevice &physicaldevice, Surface& surface) {
    QueueFamilyIndices indices = physicaldevice.findQueueFamilies(surface);

    // create set of union of all unique queue families
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        // info for creating a queues of a certain family
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    // specifies what GPU features we want to enable (just anisotropy for now)
    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    // info for creating the logical device
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    // set the device extensions we will enable
    createInfo.enabledExtensionCount = static_cast<uint32_t>(physicaldevice.deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = physicaldevice.deviceExtensions.data();

    // Deprecated, Vulkan no longer allows for seperate instance and device validation layers
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }

    // create logical device
    if (vkCreateDevice(physicaldevice.physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    // grabs and stores the queue handles
    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);

}

void LogicalDevice::destroyLogicalDevice() {
    vkDestroyDevice(device, nullptr);
}