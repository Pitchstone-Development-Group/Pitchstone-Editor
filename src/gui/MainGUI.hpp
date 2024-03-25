#pragma once

#include "GUI.hpp"
#include "SettingsGUI.hpp"
#include <string>

class MainGUI : public GUI {
public:
	MainGUI(Device *device, SDL_Renderer *renderer);
	~MainGUI();

	ImDrawData *draw(int width, int height);
	void draw(void *project, int x, int y, int width, int height, void *player) override;
private:
	int m_menu;
	SettingsGUI *m_settings;

	bool menuButton(int menu, int index, std::string name);
};