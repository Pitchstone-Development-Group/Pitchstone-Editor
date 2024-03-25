#pragma once
#include <SDL2/SDL.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer2.h>
#include "../vulkan/Device.hpp"
#include <string>
#include <vector>

class GUI {
public:
	GUI(Device *device, SDL_Renderer* renderer);
	virtual ~GUI();

	// TODO: Change project to be Project* type later on...
	virtual void draw(void *project, int x, int y, int width, int height, void *player) = 0;
protected:
	SDL_Texture* loadTexture(const std::string& filename);

	static const ImGuiWindowFlags noDecration = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar;
	static const ImGuiWindowFlags noMove = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;
	static const ImGuiWindowFlags noMoveCollapsable = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar;

	SDL_Renderer *m_renderer;
	Device *m_device;
	std::vector<SDL_Texture*> m_textures;
};