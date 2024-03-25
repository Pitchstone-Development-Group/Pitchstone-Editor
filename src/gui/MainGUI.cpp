#include "MainGUI.hpp"
#ifdef PITCHSTONE_COUNTER
#include "../sys/PerformanceCounter.hpp"
#endif
MainGUI::MainGUI(Device *device, SDL_Renderer *renderer) : GUI(device, renderer) {
	m_textures.push_back(loadTexture("images/projects.png"));
	m_textures.push_back(loadTexture("images/media.png"));
	m_textures.push_back(loadTexture("images/edit.png"));
	m_textures.push_back(loadTexture("images/effects.png"));
	m_textures.push_back(loadTexture("images/jobs.png"));
	m_textures.push_back(loadTexture("images/export.png"));
	m_textures.push_back(loadTexture("images/settings.png"));

	m_menu = 6;
	m_settings = new SettingsGUI(device, renderer);
}

MainGUI::~MainGUI() {
	delete m_settings;
}

bool MainGUI::menuButton(int menu, int index, std::string name) {
	return ImGui::ImageButton(name.c_str(), m_textures[index], ImVec2(32,32), ImVec2(0,0), ImVec2(1,1), ImVec4(0.1F, 0.1F, 0.1F, 1.0F), (menu == index) ? ImVec4(1.0F, 1.0F, 1.0F, 1.0F) : ImVec4(0.5F, 0.5F, 0.5F, 1.0F));
}

ImDrawData* MainGUI::draw(int width, int height) {
	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	
	draw(nullptr, 0, 0, width, height, nullptr);


	ImGui::Render();
	return ImGui::GetDrawData();
}

void MainGUI::draw(void *project, int x, int y, int width, int height, void *player) {
	ImGuiStyle& style = ImGui::GetStyle();
	style.FramePadding = ImVec2(0,0);
	style.WindowPadding = ImVec2(8,8);
	style.ItemSpacing = ImVec2(8,16);
	style.WindowBorderSize = 0;

	style.Colors[ImGuiCol_WindowBg] = {0.1F, 0.1F, 0.1F, 1.0F};
	style.Colors[ImGuiCol_ChildBg] = {0.1F, 0.1F, 0.1F, 1.0F};
	style.Colors[ImGuiCol_Border] = {0.0F, 0.0F, 0.0F, 1.0F};
	style.Colors[ImGuiCol_Button] = style.Colors[ImGuiCol_WindowBg];
	style.Colors[ImGuiCol_ButtonHovered] = style.Colors[ImGuiCol_WindowBg];
	style.Colors[ImGuiCol_ButtonActive] = style.Colors[ImGuiCol_WindowBg];

	ImGui::Begin("Main Menu", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowPos(ImVec2((float)x,(float)y), ImGuiCond_Once);
	ImGui::SetWindowSize(ImVec2(48.0, (float)height), ImGuiCond_Always);

	static std::string menuItems[7] = {"Project Manager", "Media Bins", "Timeline Editor", "Visual and Audial Effects", "Custom Jobs", "Export", "Settings"};

	for (int i = 0; i < 6; ++i)
		if (menuButton(m_menu, i, menuItems[i]))
			m_menu = i;
	ImGui::BeginChild("Buffer", ImVec2(32, (float)height - 352), false, ImGuiWindowFlags_NoDecoration |ImGuiWindowFlags_NoMove);
	ImGui::EndChild();
	if (menuButton(m_menu, 6, menuItems[6]))
		m_menu = 6;

	ImGui::End();

	style.Colors[ImGuiCol_WindowBg] = {0.2F, 0.2F, 0.2F, 1.0F};

	// /m_player->draw(m_project, 120, 80, 800, 400);

	switch (m_menu) {
		case 0: // Project Manager
			break;
		case 1: // Media Bins
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			break;
		default:
			m_settings->draw(project, 48, 0, width - 48, height, player);
	}
}