#ifndef PITCHSTONE_VULKAN_VULKAN
#define PITCHSTONE_VULKAN_VULKAN

#include <vulkan/vulkan.h>
#include <vector>

class Vulkan {
public:
	Vulkan(bool validator = false);
	virtual ~Vulkan();

	bool validates() { return validation; }
	VkInstance instance;
private:
	VkDebugUtilsMessengerEXT debugMessenger;
	bool validation;
};

#endif
