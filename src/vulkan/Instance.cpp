#include "Instance.hpp"
#include <SDL2/SDL_vulkan.h>
#include <iostream>

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData) {
	(void) severity;
	(void) type;
	(void) userData;

	std::cerr << "VULKAN VALIDATION LAYER: " << callbackData->pMessage << std::endl;
	return VK_FALSE;
}

static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* info_d, const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* messenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	return (func) ? func(instance, info_d, allocator, messenger) : VK_ERROR_EXTENSION_NOT_PRESENT;
}

static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* allocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
		func(instance, messenger, allocator);
}


Instance::Instance() {
	std::cout << "Instance Creation" << std::endl;
	m_instance = VK_NULL_HANDLE;
	m_debugger = VK_NULL_HANDLE;

	SDL_Vulkan_LoadLibrary(nullptr);

	VkApplicationInfo info_app{};
	info_app.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	info_app.pNext = VK_NULL_HANDLE;
	info_app.pApplicationName = "Pitchstone Editor";
	info_app.applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 0);
	info_app.pEngineName = "Pitchstone Engine";
	info_app.engineVersion = VK_MAKE_API_VERSION(0, 0, 0, 0);
	info_app.apiVersion = VK_API_VERSION_1_3;

	/* For some reason, unlike GLFW, SDL2 requires a valid window in order for the extensions to be retrived
	 * So, we'll make our won for the time being :P
	 */
	SDL_Window* window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 360, SDL_WINDOW_HIDDEN | SDL_WINDOW_VULKAN);
	uint32_t count;
	SDL_Vulkan_GetInstanceExtensions(window, &count, nullptr);
	std::vector<const char*> req_exts(count);
	SDL_Vulkan_GetInstanceExtensions(window, &count, req_exts.data());
	SDL_DestroyWindow(window);

	req_exts.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	std::vector<const char*> req_lays;
	req_lays.push_back("VK_LAYER_KHRONOS_validation");

	VkDebugUtilsMessengerCreateInfoEXT info_dbg{};
	info_dbg.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	info_dbg.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	info_dbg.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	info_dbg.pfnUserCallback = debug_callback;

	VkInstanceCreateInfo info_ins{};
	info_ins.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	info_ins.pNext = &info_dbg;
	info_ins.pApplicationInfo = &info_app;
	info_ins.flags = 0;
	info_ins.enabledExtensionCount = (uint32_t)req_exts.size();
	info_ins.ppEnabledExtensionNames = req_exts.data();
	info_ins.enabledLayerCount = (uint32_t)req_lays.size();
	info_ins.ppEnabledLayerNames = req_lays.data();
	
	if (vkCreateInstance(&info_ins, nullptr, &m_instance) != VK_SUCCESS) {
		throw std::runtime_error("VkInstance failed to be created!");
	}

	if (CreateDebugUtilsMessengerEXT(m_instance, &info_dbg, nullptr, &m_debugger) != VK_SUCCESS) {
		throw std::runtime_error("VkDebugUtilsMessengerEXT failed to be created!");
	}

	for (auto e : req_exts)
		m_extensions.push_back(e);
}


Instance::~Instance() {
	DestroyDebugUtilsMessengerEXT(m_instance, m_debugger, VK_NULL_HANDLE);
	vkDestroyInstance(m_instance, VK_NULL_HANDLE);
	SDL_Vulkan_UnloadLibrary();
	std::cout << "Instance Destruction" << std::endl;
}