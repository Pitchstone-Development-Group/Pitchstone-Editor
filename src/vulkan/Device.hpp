#ifndef PITCHSTONE_VULKAN_DEVICE
#define PITCHSTONE_VULKAN_DEVICE

#include <vulkan/vulkan.h>

class Device {
public:
	Device(VkPhysicalDevice physical, bool validation);
	virtual ~Device();

	VkDevice m_device;
	VkPhysicalDevice m_physical;
};

#endif
