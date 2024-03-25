#include "GUI.hpp"
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

GUI::GUI(Device *device, SDL_Renderer* renderer) {
	m_renderer = renderer;
	m_device = device;
}

GUI::~GUI() {
	for (auto t : m_textures) {
		SDL_DestroyTexture(t);
	}
}

SDL_Texture* GUI::loadTexture(const std::string& filename) {
	int width, height, channels;
	unsigned char* image = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);
	SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(image, width, height, 32, width * 4, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
	SDL_FreeSurface(surface);
	return texture;
}