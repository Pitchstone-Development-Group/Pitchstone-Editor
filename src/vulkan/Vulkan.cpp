#include "Vulkan.hpp"
#include <iostream>
#include <GLFW/glfw3.h>
#include <cstring>

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}

static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& info_c) {
	info_c = {};
	info_c.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	info_c.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	info_c.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	info_c.pfnUserCallback = debugCallback;
}

static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func =(PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	return (func) ? func(instance, pCreateInfo, pAllocator, pDebugMessenger) : VK_ERROR_EXTENSION_NOT_PRESENT;
}

static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
		func(instance, debugMessenger, pAllocator);
}

static bool checkValidationLayerSupport(std::vector<const char*> validationLayers) {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers)
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}

		if (!layerFound)
			return false;
	}

	return true;
}

static std::vector<const char*> getRequiredExtensions() {
	if (!glfwVulkanSupported())
		throw std::runtime_error("Vulkan loader and/or ICD not found!");
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	if (glfwExtensionCount == 0 || !glfwExtensions)
		throw std::runtime_error("GLFW required extensions not available!");
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	return extensions;
}


Vulkan::Vulkan(bool validator) {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	instance = VK_NULL_HANDLE;
	debugMessenger = VK_NULL_HANDLE;
	validation = validator;

	std::cout << "Running Vulkan constructor" << std::endl;

	if (validation && !checkValidationLayerSupport(m_layers))
		throw std::runtime_error("validation layers requested, but not available!");

	VkApplicationInfo info_a{};
	info_a.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	info_a.pApplicationName = "Pitchstone Editor";
	info_a.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
	info_a.pEngineName = "Pitchstone";
	info_a.engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
	info_a.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo info_c{};
	info_c.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	info_c.pApplicationInfo = &info_a;

	auto extensions = getRequiredExtensions();
	info_c.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	info_c.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT info_d{};

	if (validation) {
		info_c.enabledLayerCount = static_cast<uint32_t>(m_layers.size());
		info_c.ppEnabledLayerNames = m_layers.data();
		populateDebugMessengerCreateInfo(info_d);
		info_c.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &info_d;
	} else {
		info_c.enabledLayerCount = 0;
		info_c.pNext = nullptr;
	}

	if (vkCreateInstance(&info_c, nullptr, &instance) != VK_SUCCESS)
		throw std::runtime_error("failed to create instance!");

	if (validation) {
		VkDebugUtilsMessengerCreateInfoEXT info_D;
		populateDebugMessengerCreateInfo(info_D);

		if (CreateDebugUtilsMessengerEXT(instance, &info_D, nullptr, &debugMessenger) != VK_SUCCESS)
			throw std::runtime_error("failed to set up debug messenger!");
	}
	std::cout << "Finished Vulkan constructor" << std::endl;
}

Vulkan::~Vulkan() {
	std::cout << "Running Vulkan destructor" << std::endl;
	DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	vkDestroyInstance(instance, nullptr);
	glfwTerminate();
	std::cout << "Finished Vulkan destructor" << std::endl;
}
