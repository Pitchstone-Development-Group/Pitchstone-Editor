#pragma once

#include "Device.hpp"
#include "ImGui_13.hpp"
#include "Instance.hpp"
#include <SDL2/SDL.h>


class Window {
public:
	Window(Instance *instance, int width, int height, const std::string& title);
	~Window();

	VkSurfaceKHR surface() { return m_surface; }

	void draw();
	void present() { present(false); }

	void setupImgui(Device *device);

	static int event(void*, SDL_Event *event);
	static bool update();

private:
	void present(bool resize);

	SDL_Window *m_window;
	VkSurfaceKHR m_surface;
	ImGui_13 *m_imgui;
	
	VkDescriptorPool m_descriptorPool;
	VkPipelineCache m_cache = VK_NULL_HANDLE;

	Device *m_device;
	Queue *m_present, *m_graphics;

	VkInstance m_instance;

	bool m_rebuild = false;
};