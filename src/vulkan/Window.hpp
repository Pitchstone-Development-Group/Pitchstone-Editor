#ifndef PITCHSTONE_VULKAN_WINDOW
#define PITCHSTONE_VULKAN_WINDOW

#include "Vulkan.hpp"
#include <GLFW/glfw3.h>
#include <string>

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

	void setSizeLimits(int minw, int minh, int maxw, int maxh) { glfwSetWindowSizeLimits(m_window, minw, minh, maxw, maxh); }
	void attachDevice(VkPhysicalDevice physical, VkDevice device, int images);
	void switchContext();

	inline VkInstance getInstance() { return m_instance; }
	inline VkSurfaceKHR getSurface() { return m_surface; }

	static void callbackKey(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void callbackChar(GLFWwindow* window, unsigned int unicode);
	static void callbackPos(GLFWwindow* window, double xpos, double ypos);
	static void callbackEnter(GLFWwindow* window, int entered);
	static void callbackButton(GLFWwindow* window, int button, int action, int mods);
	static void callbackScroll(GLFWwindow* window, double xoffset, double yoffset);
	
	static void callbackResize(GLFWwindow* window, int width, int height);

	KeyboardMouse m_keymouse;
	bool m_closed = false, m_hidden = false, m_maximized = false;
	int m_width, m_height;
	bool m_resized = false;

	/* Needed to prevent cyclic dependency and for swapchain :) */
	VkInstance m_instance;
	GLFWwindow* m_window;
	VkSurfaceKHR m_surface;
};

#endif