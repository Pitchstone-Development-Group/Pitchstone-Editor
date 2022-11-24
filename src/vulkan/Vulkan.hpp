#ifndef OBSIDIAN_VULKAN_VULKAN
#define OBSIDIAN_VULKAN_VULKAN

#include <vulkan/vulkan.h>
#include <vector>

class Vulkan {
public:
	Vulkan(bool validator = false);
	virtual ~Vulkan();

	bool validates() { return validation; }
private:
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	bool validation;
};

#endif
