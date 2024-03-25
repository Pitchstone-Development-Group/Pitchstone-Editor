#pragma once

#include "GUI.hpp"

class SettingsGUI : public GUI {
public:
	SettingsGUI(Device *device, SDL_Renderer *renderer);
	~SettingsGUI();

	void draw(void *project, int x, int y, int width, int height, void *player) override;
};