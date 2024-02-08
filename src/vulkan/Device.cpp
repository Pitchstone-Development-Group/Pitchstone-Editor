#include "Device.hpp"
#include "Queue.hpp"
#include <iostream>

DeviceProperties::DeviceProperties() {
	properties_10 = {};
	properties_10.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
	properties_10.pNext = &properties_11;
	properties_11 = {};
	properties_11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;
	properties_11.pNext = &properties_12;
	properties_12 = {};
	properties_12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;
	properties_12.pNext = &properties_13;
	properties_13 = {};
	properties_13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES;

	features_10 = {};
	features_10.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	features_10.pNext = &features_11;
	features_11 = {};
	features_11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
	features_11.pNext = &features_12;
	features_12 = {};
	features_12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	features_12.pNext = &features_13;
	features_13 = {};
	features_13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
}

DeviceProperties::DeviceProperties(VkPhysicalDevice physical, bool blank) : DeviceProperties() {
	vkGetPhysicalDeviceProperties2(physical, &properties_10);

	if (blank) {
		vkGetPhysicalDeviceFeatures2(physical, &features_10);
	}

	vkGetPhysicalDeviceMemoryProperties(physical, &memory);
}

DeviceProperties::~DeviceProperties() {

}

Device::Device(Instance *instance, VkSurfaceKHR surface) {
	(void) surface;

	m_device = VK_NULL_HANDLE;
	m_instance = instance->instance();
	uint32_t count;
	vkEnumeratePhysicalDevices(m_instance, &count, VK_NULL_HANDLE);
	m_physicals.resize(count);
	vkEnumeratePhysicalDevices(m_instance, &count, m_physicals.data());

	m_physical = 0;
	
	auto queueAssignments = Queue::assign(m_physicals[m_physical], surface);

	auto queueFamilies = Queue::allocation(queueAssignments);

	std::vector<float> priorities(QUEUE_THREADS);
	for (float& p : priorities) {
		p = 1.0F;
	}

	std::vector<VkDeviceQueueCreateInfo> info_que(queueFamilies.size());
	uint32_t index = 0;
	for (auto& qf : queueFamilies) {
		info_que[index].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		info_que[index].pNext = VK_NULL_HANDLE;
		info_que[index].flags = 0;
		info_que[index].queueFamilyIndex = qf.first;
		info_que[index].queueCount = qf.second;
		info_que[index].pQueuePriorities = priorities.data();
		index++;
	}
	

	std::vector<const char*> extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	VkDeviceCreateInfo info_dev{};
	info_dev.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	info_dev.pNext = &m_properties.features_11;
	info_dev.flags = 0;
	info_dev.queueCreateInfoCount = (uint32_t)info_que.size();
	info_dev.pQueueCreateInfos = info_que.data();
	info_dev.enabledLayerCount = 0;
	info_dev.ppEnabledLayerNames = VK_NULL_HANDLE;
	info_dev.enabledExtensionCount = (uint32_t)extensions.size();
	info_dev.ppEnabledExtensionNames = extensions.data();
	info_dev.pEnabledFeatures = &m_properties.features_10.features;

	vkCreateDevice(m_physicals[m_physical], &info_dev, VK_NULL_HANDLE, &m_device);

	for (int i = 0; i < QUEUE_THREADS; ++i) {
		m_queuesIndex[i] = QUEUE_THREADS;
	}
	int unique = 0;
	for (int i = 0; i < QUEUE_THREADS; ++i) {
		if (m_queuesIndex[i] != QUEUE_THREADS) {
			continue;
		}
		bool alone = true;
		for (int j = i + 1; j < QUEUE_THREADS; ++j) {
			if (queueAssignments[i].first == queueAssignments[j].first && queueAssignments[i].second == queueAssignments[j].second) {
				m_queuesIndex[j] = unique;

				if (i == QUEUE_WINDOW_PRESENT && j == QUEUE_WINDOW_GRAPHICS) {
					continue;
				} else if (i == QUEUE_CONVERTOR_COMPUTE && j == QUEUE_CONVERTOR_TRANSFER) {
					continue;
				} else if (i == QUEUE_ENGINE_GRAPHICS && j == QUEUE_ENGINE_COMPUTE) {
					continue;
				}

				alone = false;
			}
		}
		m_queuesIndex[i] = unique;
		m_queues.push_back(new Queue(m_device, queueAssignments[i].first, queueAssignments[i].second, alone));
		unique++;
	}
}

Device::~Device() {
	for (Queue* queue : m_queues) {
		delete queue;
	}
	vkDestroyDevice(m_device, VK_NULL_HANDLE);
}