#include "SettingsGUI.hpp"
#include "../sys/System.hpp"
#include <inttypes.h>
#include <bit>

SettingsGUI::SettingsGUI(Device *device, SDL_Renderer *renderer) : GUI(device, renderer) {
	m_textures.push_back(loadTexture("images/cpu.png"));
	m_textures.push_back(loadTexture("images/gpu.png"));
	m_textures.push_back(loadTexture("images/ram.png"));
	m_textures.push_back(loadTexture("images/monitors.png"));
}

SettingsGUI::~SettingsGUI() {
	
}

void SettingsGUI::draw(void *project, int x, int y, int width, int height, void *player) {
	(void) player;
	(void) project;

	union {
		uint32_t l[12];
		uint8_t b[48];
	} u, m, em;

	System::cpuid(0x0, 0, &m.l[0], &m.l[1], &m.l[2], &m.l[3]);
	System::cpuid(0x80000000, 0, &em.l[0], &em.l[1], &em.l[2], &em.l[3]);

	uint32_t total, avail, curr;
	System::getRAM(&total, &avail, &curr);

	ImGui::Begin("Settings", nullptr, GUI::noMove | GUI::noDecration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysVerticalScrollbar);
	ImGui::SetWindowPos(ImVec2((float)x,(float)y), ImGuiCond_Once);
	ImGui::SetWindowSize(ImVec2((float)width, (float)height), ImGuiCond_Always);

	ImGui::BeginTable("Hardware Information", 2);
	ImGui::TableSetupColumn("Icon", ImGuiTableColumnFlags_WidthFixed, 93.0f);
	ImGui::TableSetupColumn("Information", ImGuiTableColumnFlags_WidthFixed, 500.0f);

	ImGui::TableNextRow();

	ImGui::TableSetColumnIndex(0);
	ImGui::Image(m_textures[0], ImVec2(48,48));
	ImGui::TableSetColumnIndex(1);
	if (em.l[0] >= 0x80000004) {
		System::cpuid(0x80000002, 0, &u.l[0], &u.l[1], &u.l[2], &u.l[3]);
		System::cpuid(0x80000003, 0, &u.l[4], &u.l[5], &u.l[6], &u.l[7]);
		System::cpuid(0x80000004, 0, &u.l[8], &u.l[9], &u.l[10], &u.l[11]);
		ImGui::Text("Central Processor Name: %s", (char*) u.b);
	} else {
		System::cpuid(0x0, 0, &u.l[0], &u.l[1], &u.l[2], &u.l[3]);
		uint32_t t = u.l[2];
		u.l[2] = u.l[3];
		u.l[3] = t;
		ImGui::Text("Central Processor Manufacturer ID: %.12s", (char*) &u.b[4]);
	}
	if (m.l[0] >= 0x16) {
		System::cpuid(0x16, 0, &u.l[0], &u.l[1], &u.l[2], &u.l[3]);
		ImGui::Text("Base Freq (MHz): %" PRIu32 " | Max Freq (MHz): %" PRIu32 " | Bus Freq (MHz): %" PRIu32, u.l[0], u.l[1], u.l[2]);
	} else {
		ImGui::Text("Base, Max, and Bus Frequencies Unknown");
	}
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Text(" ");

	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);
	ImGui::Image(m_textures[1], ImVec2(77,48));
	ImGui::TableSetColumnIndex(1);
	ImGui::Text("Graphics Processor Name: %s", m_device->properties().properties_10.properties.deviceName);
	uint32_t deviceApi = m_device->properties().properties_10.properties.apiVersion;
	uint32_t instanceApi;
	vkEnumerateInstanceVersion(&instanceApi);

	ImGui::Text("Max Instance API Version: %" PRIu32 ".%" PRIu32 ".%" PRIu32, VK_API_VERSION_MAJOR(instanceApi), VK_API_VERSION_MINOR(instanceApi), VK_API_VERSION_PATCH(instanceApi));
	ImGui::Text("Max Device API Version: %" PRIu32 ".%" PRIu32 ".%" PRIu32, VK_API_VERSION_MAJOR(deviceApi), VK_API_VERSION_MINOR(deviceApi), VK_API_VERSION_PATCH(deviceApi));
	static std::string types[5] = {"Other", "Integrated GPU", "Discrete GPU", "Virtual GPU", "CPU"};
	ImGui::Text("Graphics Processor Type: %s", types[m_device->properties().properties_10.properties.deviceType].c_str());
	ImGui::Text(" ");

	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);
	ImGui::Image(m_textures[2], ImVec2(75,48));
	ImGui::TableSetColumnIndex(1);
	ImGui::Text("Total Memory (MB): %" PRIu32, total);
	ImGui::Text("Available Memory (MB): %" PRIu32, avail);
	ImGui::Text("Current Memory Usage (MB): %" PRIu32, curr);
	ImGui::Text(" ");

	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);
	ImGui::Image(m_textures[3], ImVec2(48,48));
	ImGui::TableSetColumnIndex(1);
	int count = SDL_GetNumVideoDisplays();
	for (int i = 0 ; i < count; ++i) {
		SDL_DisplayMode mode;
		SDL_GetCurrentDisplayMode(i, &mode);
		SDL_PixelFormat* format = SDL_AllocFormat(mode.format);
		ImGui::Text("Monitor %d: %dx%d RGB %d:%d:%d @ %dHz", i + 1, mode.w, mode.h, std::popcount(format->Rmask), std::popcount(format->Gmask), std::popcount(format->Bmask), mode.refresh_rate);
	}

	ImGui::EndTable();

	ImGui::End();
}