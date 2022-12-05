#include "Window.hpp"
#include <vector>
#include <algorithm>
#include <iostream>

Window::Window(Vulkan *vulkan, int width, int height, std::string title) {
	m_window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
	m_closed = m_hidden = m_maximized = false;

	glfwSetKeyCallback(m_window, callbackKey);
	glfwSetCharCallback(m_window, callbackChar);
	glfwSetCursorPosCallback(m_window, callbackPos);
	glfwSetCursorEnterCallback(m_window, callbackEnter);
	glfwSetMouseButtonCallback(m_window, callbackButton);
	glfwSetScrollCallback(m_window, callbackScroll);
	glfwSetFramebufferSizeCallback(m_window, callbackResize);
	glfwSetWindowUserPointer(m_window, this);
	
	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1);
	m_width = width, m_height = height;
	m_instance = vulkan->instance;
	VkResult ret = glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface);
	if (ret != VK_SUCCESS)
		throw std::runtime_error("Failed to create window surface! Code: " + std::to_string((int)ret));
}

Window::~Window() {
	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	glfwDestroyWindow(m_window);
	m_window = NULL;
}

/*******************************************************
 **********  LIST OF ALL CALLBACKS BELOW ***************
 *******************************************************/

void Window::callbackKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

void Window::callbackChar(GLFWwindow* window, unsigned int unicode) {

}

void Window::callbackPos(GLFWwindow* window, double xpos, double ypos) {

}

void Window::callbackEnter(GLFWwindow* window, int entered) {

}

void Window::callbackButton(GLFWwindow* window, int button, int action, int mods) {

}

void Window::callbackScroll(GLFWwindow* window, double xoffset, double yoffset) {

}


void Window::callbackResize(GLFWwindow* window, int width, int height) {
	Window *win = (Window*) glfwGetWindowUserPointer(window);
	if (win->m_width == width && win->m_height == height) return;
	//glViewport(0, 0, width, height);
	win->m_width = width;
	win->m_height = height;
	win->m_resized = true;
	//glfwSwapBuffers(win->window);
}
