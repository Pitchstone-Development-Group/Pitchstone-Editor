#pragma once

#include <SDL2/SDL.h>
#include <string>

class Window {
public:
	Window(int width, int height, const std::string& title);
	~Window();

	void draw();

	static int event(void*, SDL_Event *event);
	static bool update();

private:
	SDL_Window *m_window;
	SDL_Renderer *m_renderer;
};