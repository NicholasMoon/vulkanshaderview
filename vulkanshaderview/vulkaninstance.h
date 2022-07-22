#ifndef VULKANINSTANCE_H
#define VULKANINSTANCE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>


class VulkanInstance {
public:
	VulkanInstance();
	~VulkanInstance();

	// create vulkan instance
	void createInstance();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	// sets up the debug messenger handle and set debug callback parameters
	void setupDebugMessenger();

	// gets required extensions for glfw and validation callbacks (if DEBUG)
	std::vector<const char*> getRequiredExtensions();

	// check that requested validation layers are supported
	bool checkValidationLayerSupport();

	VkInstance instance; // Vulkan Instance
	VkDebugUtilsMessengerEXT debugMessenger; // Debug Messenger

#define DEBUG
#ifndef DEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	// name of validation layers to enable in debug mode
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};
private:

};

// function to get and enable extension required to utilize the debug messenger and callbacks
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pDebugMessenger);

// function to disable extension required to utilize the debug messenger and callbacks
void DestroyDebugUtilsMessengerEXT(VkInstance instance,
	VkDebugUtilsMessengerEXT debugMessenger,
	const VkAllocationCallbacks* pAllocator);

// function to print validation callback to standard error
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData);

#endif
