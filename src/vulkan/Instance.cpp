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

	VkInstanceCreateInfo info_ins{};
	info_ins.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	info_ins.pApplicationInfo = &info_app;
	info_ins.flags = 0;
	
	if (vkCreateInstance(&info_ins, nullptr, &m_instance) != VK_SUCCESS) {
		throw std::runtime_error("VkInstance failed to be created!");
	}
}


Instance::~Instance() {
	vkDestroyInstance(m_instance, VK_NULL_HANDLE);
}