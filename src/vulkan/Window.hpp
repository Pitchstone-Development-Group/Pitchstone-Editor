#pragma once

#include <SDL2/SDL.h>
#include "Device.hpp"
#include "../gui/MainGUI.hpp"
#include <string>

class Window {
public:
	Window(Device *device, int width, int height, const std::string& title);
	~Window();

	void draw();

	static int event(void*, SDL_Event *event);
	static bool update();

private:
	SDL_Window *m_window;
	SDL_Renderer *m_renderer;
	MainGUI *m_gui;
	Device *m_device;
};