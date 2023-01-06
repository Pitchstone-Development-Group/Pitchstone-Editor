#ifndef PITCHSTONE_VULKAN_VULKAN
#define PITCHSTONE_VULKAN_VULKAN

#include <vulkan/vulkan.h>
#include <vector>

class Vulkan {
public:
	Vulkan(bool validator = false);
	virtual ~Vulkan();

	bool validates() { return m_validation; }
	VkInstance m_instance;
	std::vector<const char*> m_layers = { "VK_LAYER_KHRONOS_validation" };
private:
	VkDebugUtilsMessengerEXT debugMessenger;
	bool m_validation;
};

#endif
