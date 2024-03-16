#pragma once

#include "Allocator.hpp"
#include "Instance.hpp"
#include "Queue.hpp"
#include <unordered_map>

class DeviceProperties{
public:
	DeviceProperties();
	DeviceProperties(VkPhysicalDevice, bool blank);
	~DeviceProperties();

	VkPhysicalDeviceProperties2 properties_10;
	VkPhysicalDeviceVulkan11Properties properties_11;
	VkPhysicalDeviceVulkan12Properties properties_12;
	VkPhysicalDeviceVulkan13Properties properties_13;

	VkPhysicalDeviceFeatures2 features_10;
	VkPhysicalDeviceVulkan11Features features_11;
	VkPhysicalDeviceVulkan12Features features_12;
	VkPhysicalDeviceVulkan13Features features_13;
	
	VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT features_page;

	VkPhysicalDeviceMemoryProperties memory;

	std::vector<VkQueueFamilyProperties> families;
	std::vector<bool> presentation;

	std::unordered_map<VkFormat, VkFormatProperties3> formats;
};

class Device {
public:
	Device(Instance*, VkSurfaceKHR);
	~Device();

	VkInstance instance() { return m_instance; }
	VkDevice device() { return m_device; }
	VkPhysicalDevice physical() { return m_physicals[m_physical]; }
	Queue* queue(QueueThread thread) { return m_queues[m_queuesIndex[thread]]; }
	/* TODO: Create more rigurous dependency system to render this function obsolete */
	bool deviceMemoryPriority() { return m_deviceMemoryPriority; };

	void localSizes(uint32_t width, uint32_t height, uint32_t *sizeX, uint32_t *sizeY);

	VkResult create(VkDeviceSize size, VkDeviceMemory *memory, VkMemoryPropertyFlags flags);
	VkResult create(VkBufferCreateInfo* info, VkBuffer* buffer, VkMemoryPropertyFlags flags, bool alone);
	VkResult create(VkImageCreateInfo* info, VkImage *image, VkMemoryPropertyFlags flags, bool alone);

	void* map(VkBuffer buffer);
	void unmap(VkBuffer buffer);

	void destroy(VkDeviceMemory heap);
	void destroy(VkBuffer buffer);
	void destroy(VkImage image);
private:
	std::vector<VkPhysicalDevice> m_physicals;
	uint32_t m_physical;

	DeviceProperties m_properties;

	uint32_t m_queuesIndex[QUEUE_THREADS];
	std::vector<Queue*> m_queues;

	VkDevice m_device;
	VkInstance m_instance;

	Allocator *m_allocator;

	bool m_deviceMemoryPriority;
};