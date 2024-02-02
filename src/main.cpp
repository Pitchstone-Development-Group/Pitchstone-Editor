// Pitchstone-Editor.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <SDL2/SDL.h>
#include "vulkan/Instance.hpp"
#include "vulkan/Device.hpp"

using namespace std;

int main(int argc, char* argv[]) {
	(void)argc;
	(void)argv;

	SDL_Init(SDL_INIT_EVERYTHING);
	cout << "Hello CMake." << endl;
	SDL_Quit();

	Instance* instance = new Instance();
	Device* device = new Device(instance);

	delete device;
	delete instance;
	return 0;
}
