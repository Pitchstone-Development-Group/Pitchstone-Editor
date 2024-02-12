#pragma once

#include "Instance.hpp"
#include "Device.hpp"
#include <SDL2/SDL.h>
#include <imgui/imgui_impl_vulkan.h>

class Window {
public:
	Window(Instance *instance, int width, int height, const std::string& title);
	~Window();

	VkSurfaceKHR surface() { return m_surface; }

	void draw();

	void setupImgui(Device *device);

	static int event(void*, SDL_Event *event);
	static bool update();

private:

	void render(ImDrawData *drawData);

	SDL_Window *m_window;
	VkSurfaceKHR m_surface;
	ImGui_ImplVulkanH_Window m_imgui;
	
	VkDescriptorPool m_descriptorPool;
	VkPipelineCache m_cache = VK_NULL_HANDLE;

	Device *m_device;
	Queue *m_present, *m_graphics;

	VkInstance m_instance;

	bool m_rebuild = false;
};