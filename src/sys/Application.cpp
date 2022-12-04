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
	vkEnumeratePhysicalDevices(m_instance->instance, &count, nullptr);
	physicals.resize(count);
	std::cout << count << " devices found!" << std::endl;
	vkEnumeratePhysicalDevices(m_instance->instance, &count, physicals.data());

	m_window = new Window(m_instance, 640, 480, "Pitchstone Project");

	m_manager = new QueueManager(physicals[0], m_window->getSurface());
	m_device = new Device(physicals[0], m_instance, m_manager);
}

Application::~Application() {
	//delete engine;
	delete m_manager;
	delete m_device;
	delete m_window;
	delete m_instance;
}

void threadWindow(Application *app) {

}

void threadEngine(Application* app) {

}