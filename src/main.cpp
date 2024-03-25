// Pitchstone-Editor.cpp : Defines the entry point for the application.
//
#include <SDL2/SDL.h>
#include <iostream>
#include "vulkan/Instance.hpp"
#include "vulkan/Window.hpp"
#include "vulkan/Device.hpp"
#include "vulkan/Image.hpp"

int main(int argc, char* argv[]) {
	(void)argc;
	(void)argv;

	SDL_Init(SDL_INIT_EVERYTHING);
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif
	SDL_SetEventFilter(Window::event, nullptr);

	Instance* instance = new Instance();
	Device* device = new Device(instance);
	Window* window = new Window(device, 800, 600, "Pitchstone Editor");

	while (Window::update() == false) {
		window->draw();
	}

	delete window;
	delete device;
	delete instance;

	SDL_Quit();

	return 0;
}
