#include "Instance.hpp"
#include <SDL2/SDL_vulkan.h>
#include <iostream>

Instance::Instance() {
	m_instance = VK_NULL_HANDLE;

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
	 * So, we'll make our own for the time being :P
	 */
	SDL_Window* window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 360, SDL_WINDOW_HIDDEN | SDL_WINDOW_VULKAN);
	uint32_t count;
	SDL_Vulkan_GetInstanceExtensions(window, &count, nullptr);
	std::vector<const char*> req_exts(count);
	SDL_Vulkan_GetInstanceExtensions(window, &count, req_exts.data());
	SDL_DestroyWindow(window);

	req_exts.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	VkInstanceCreateInfo info_ins{};
	info_ins.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	info_ins.pApplicationInfo = &info_app;
	info_ins.flags = 0;
	info_ins.enabledExtensionCount = (uint32_t)req_exts.size();
	info_ins.ppEnabledExtensionNames = req_exts.data();
	
	if (vkCreateInstance(&info_ins, nullptr, &m_instance) != VK_SUCCESS) {
		throw std::runtime_error("VkInstance failed to be created!");
	}

	for (auto e : req_exts)
		m_extensions.push_back(e);
}


Instance::~Instance() {
	vkDestroyInstance(m_instance, VK_NULL_HANDLE);
	SDL_Vulkan_UnloadLibrary();
}