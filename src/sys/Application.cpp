#include "Application.hpp"

void threadWindow(Application *app); 
void threadEngine(Application *app); 

Application::Application() {
	m_instance = new Vulkan(true);

	std::vector<VkPhysicalDevice> physicals;

	/* Count and then list physical devices available, then choose the first one
	 * TODO: Grab the list of gpus, and find the most suitable for the application
	 */
	uint32_t count;
	vkEnumeratePhysicalDevices(m_instance->m_instance, &count, nullptr);
	physicals.resize(count);
	std::cout << count << " devices found!" << std::endl;
	vkEnumeratePhysicalDevices(m_instance->m_instance, &count, physicals.data());

	m_window = new Window(m_instance, 640, 480, "Pitchstone Project");

	m_manager = new QueueManager(physicals[0], m_window->getSurface());
	m_device = new Device(physicals[0], m_instance, m_manager);

	m_window->setupImgui(m_device, m_manager->getQueue(0));

	bool showDemoWindow = true;
	bool showAnotherWindow = false;
	int counter = 0;
	float f = 0;
	ImVec4 clearColor = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);

	while (!glfwWindowShouldClose(m_window->m_window)) {
		m_window->update();
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		if (showDemoWindow)
			ImGui::ShowDemoWindow(&showDemoWindow);

		ImGui::Begin("Pitchstone Editor", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);						 // Create a window called "Hello, world!" and append into it.

		ImGui::Text("Using ImGui example to prove integration works");
		ImGui::Checkbox("Demo Window", &showDemoWindow);
		ImGui::Checkbox("Another Window", &showAnotherWindow);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		ImGui::ColorEdit3("clear color", (float*)&clearColor);

		if (ImGui::Button("Button"))
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
		if (showAnotherWindow)
		{
			ImGui::Begin("Another Window", &showAnotherWindow);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				showAnotherWindow = false;
			ImGui::End();
		}
		ImGui::Render();
		m_window->draw(ImGui::GetDrawData());
	}
}

Application::~Application() {
	//delete engine;
	delete m_window;
	delete m_manager;
	delete m_device;
	delete m_instance;
}

void threadWindow(Application *app) {

}

void threadEngine(Application* app) {

}