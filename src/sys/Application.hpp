#ifndef PITCHSTONE_SYS_APPLICATION
#define PITCHSTONE_SYS_APPLICATION

#include "../engine/Engine.hpp"
#include "../vulkan/Vulkan.hpp"
#include "../vulkan/Device.hpp"
#include "../vulkan/QueueManager.hpp"
#include "../vulkan/Window.hpp"
#include <thread>

class Application {
public:
	Application();
	virtual ~Application();
	void run();
private:
	Vulkan *m_instance;
	Device *m_device;
	Window *m_window;
	QueueManager *m_manager;
	Engine *m_engine;
	std::thread *m_twindow, *m_tengine;
};

#endif
