#ifndef PITCHSTONE_VULKAN_WINDOW
#define PITCHSTONE_VULKAN_WINDOW

#include "Vulkan.hpp"
#include "Device.hpp"
#include "Queue.hpp"
#include <GLFW/glfw3.h>
#include <string>
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_vulkan.h"

class KeyboardMouse {
public:
	KeyboardMouse() {
		for (int i = 0; i <= GLFW_KEY_LAST; ++i) keys[i] = false; //TODO: FIND NUMBER OF KEY CODES
		for (int i = 0; i < 32; ++i) mouse.buttons[i] = false;
	}

	bool keys[GLFW_KEY_LAST + 1];
	struct { int x, y, xrel, yrel; bool buttons[32]; } mouse;
};

class Window {
public:
	Window(Vulkan *vulkan, int, int, std::string);
	virtual ~Window();

	void update();
	void draw(ImDrawData* drawData);

	void setSizeLimits(int minw, int minh, int maxw, int maxh) { glfwSetWindowSizeLimits(m_window, minw, minh, maxw, maxh); }
	void setupImgui(Device* device, Queue* queue);

	inline VkSurfaceKHR getSurface() { return m_surface; }

	KeyboardMouse m_keymouse;
	bool m_closed = false, m_hidden = false, m_maximized = false;
	int m_width, m_height;
	bool m_resized = false;

	/* Needed to prevent cyclic dependency and for swapchain :) */
	Vulkan *m_vulkan;
	Device *m_device;
	Queue *m_queue;

	GLFWwindow* m_window;
	VkSurfaceKHR m_surface;
	ImGui_ImplVulkanH_Window m_imgui;

	VkDescriptorPool m_descriptorPool;
	VkPipelineCache m_cache = nullptr;
private:
	void render(ImDrawData *drawData);
	bool m_rebuild = false;
};

#endif