// Pitchstone-Editor.cpp : Defines the entry point for the application.
//
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <iostream>
#include "vulkan/Instance.hpp"
#include "vulkan/Window.hpp"
#include "vulkan/Device.hpp"
#include <imgui/imgui_impl_sdl2.h>
#include "vulkan/ImGui_13.hpp"

using namespace std;

volatile bool rendering = true;

static void window_thread(Window *window) {
	while (rendering) {
		window->draw();
	}	
}

int main(int argc, char* argv[]) {
	(void)argc;
	(void)argv;

	std::cout << "Here!" << std::endl;

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_SetEventFilter(Window::event, nullptr);

	std::cout << "Here!" << std::endl;

	Instance* instance = new Instance();
	Window* window = new Window(instance, 800, 600, "Pitchstone Editor");
	Device* device = new Device(instance, window->surface());

	window->setupImgui(device);

	std::thread looper(window_thread, window);

	while (Window::update() == false) {
		window->present();
	}

	rendering = false;
	looper.join();

	SDL_Delay(1000);

	delete window;
	delete device;
	delete instance;

	SDL_Quit();

	std::cout << "Exiting successfully!!!" << std::endl;

	return 0;
}
