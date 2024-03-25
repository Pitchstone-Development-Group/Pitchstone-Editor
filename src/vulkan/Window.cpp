#include "Window.hpp"
#include <SDL2/SDL_vulkan.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer2.h>
#include <iostream>

Window::Window(Device *device, int width, int height, const std::string& title) {
	m_device = device;
	m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
	SDL_SetWindowMinimumSize(m_window, 600, 360);
	SDL_SetWindowData(m_window, "this", this);

	SDL_Surface *icon = SDL_LoadBMP("icon.bmp");
	SDL_SetWindowIcon(m_window, icon);
	SDL_FreeSurface(icon);

	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.IniFilename = NULL;
	io.LogFilename = NULL;
	ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(m_window, m_renderer);
    ImGui_ImplSDLRenderer2_Init(m_renderer);

	m_gui = new MainGUI(device, m_renderer);
}

Window::~Window() {
	delete m_gui;
	ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
}

void Window::draw() {
	int width, height;
	SDL_GetWindowSize(m_window, &width, &height);

	// ImGui_ImplSDLRenderer2_NewFrame();
	// ImGui_ImplSDL2_NewFrame();
	// ImGui::NewFrame();

	// ImGui::ShowDemoWindow();

	// ImGui::Begin("Oops!!!", nullptr, 0);
	// ImGui::SetWindowSize({80, 80}, 0);
	// ImGui::BeginChild("Whatever", {60, 60}, 0, 0);
	// ImGui::GetWindowDrawList()->AddLine({0, 0}, {8,8}, IM_COL32(255, 255, 255, 255), 3.0F);
	// ImGui::EndChild();
	// ImGui::End();

	// ImGui::EndFrame();

	// ImGui::Render();

	auto drawData = m_gui->draw(width, height);

	ImGuiIO& io = ImGui::GetIO();
	SDL_RenderSetScale(m_renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
	SDL_SetRenderDrawColor(m_renderer, 26, 26, 26, 255);
	SDL_RenderClear(m_renderer);
	ImGui_ImplSDLRenderer2_RenderDrawData(drawData);
	SDL_RenderPresent(m_renderer);
}

/* Pay attention to this function and event handling as this may run outside the main thread! */
int Window::event(void *userdata, SDL_Event *event) {
	(void)userdata;
	if (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_RESIZED) {
		Window *window = (Window*) SDL_GetWindowData(SDL_GetWindowFromID(event->window.windowID), "this");
		window->draw();
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