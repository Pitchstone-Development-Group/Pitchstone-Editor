// Pitchstone-Editor.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <SDL2/SDL.h>
#include "vulkan/Instance.hpp"
#include "vulkan/Window.hpp"
#include "vulkan/Device.hpp"
#include <imgui/imgui_impl_sdl2.h>

using namespace std;

int main(int argc, char* argv[]) {
	(void)argc;
	(void)argv;

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_SetEventFilter(Window::event, nullptr);

	Instance* instance = new Instance();
	Window* window = new Window(instance, 800, 600, "Pitchstone Editor");
	Device* device = new Device(instance, window->surface());

	window->setupImgui(device);

    while(!Window::update()) {
		Window::update();
		window->draw();
	}

	delete window;
	delete device;
	delete instance;

	SDL_Quit();

	return 0;
}
