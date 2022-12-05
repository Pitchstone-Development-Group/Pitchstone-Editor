#ifndef PITCHSTONE_VULKAN_DEVICE
#define PITCHSTONE_VULKAN_DEVICE

#include <vulkan/vulkan.h>
#include "QueueManager.hpp"
#include "Vulkan.hpp"

class Device {
public:
	Device(VkPhysicalDevice physical, Vulkan *instance, QueueManager* manager);
	virtual ~Device();

	void wait();

	VkDevice m_device;
	VkPhysicalDevice m_physical;
	std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
};

#endif
