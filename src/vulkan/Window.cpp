#include "Window.hpp"
#include <SDL2/SDL_vulkan.h>
#include <imgui/imgui_impl_sdl2.h>
#include <iostream>

Window::Window(Instance *instance, int width, int height, const std::string& title) {
	m_instance = instance->instance();
	m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
	SDL_Vulkan_CreateSurface(m_window, m_instance, &m_surface);
	SDL_SetWindowData(m_window, "this", this);

	SDL_Surface *icon = SDL_LoadBMP("icon.bmp");
	SDL_SetWindowIcon(m_window, icon);
	SDL_FreeSurface(icon);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForVulkan(m_window);
}

Window::~Window() {
	delete m_imgui;
	ImGui::DestroyContext();
	vkDestroySurfaceKHR(m_instance, m_surface, VK_NULL_HANDLE);
	SDL_DestroyWindow(m_window);
}

void Window::draw() {
	int width, height;
	SDL_GetWindowSize(m_window, &width, &height);

	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();

	ImGui::Render();
	ImDrawData* drawData = ImGui::GetDrawData();

	m_imgui->draw(drawData);
}

void Window::present(bool resize) {
	int width, height;
	SDL_GetWindowSize(m_window, &width, &height);
	m_imgui->present(resize, (uint32_t)width, (uint32_t)height);
}

void Window::setupImgui(Device *device) {
	m_device = device;
	int width, height;
	SDL_GetWindowSize(m_window, &width, &height);
	m_imgui = new ImGui_13(device, m_surface, (uint32_t)width, (uint32_t)height);
}

/* Pay attention to this function and event handling as this may run outside the main thread! */
int Window::event(void *userdata, SDL_Event *event) {
	(void)userdata;
	if (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_RESIZED || event->window.event == SDL_WINDOWEVENT_EXPOSED) {
		Window *window = (Window*) SDL_GetWindowData(SDL_GetWindowFromID(event->window.windowID), "this");
		window->present(true);
		return 0;
	}
	return 1;
}

bool Window::update() {
	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		ImGui_ImplSDL2_ProcessEvent(&event);
		if(event.type == SDL_QUIT) {
			return true;
		}
	}
	return false;
}