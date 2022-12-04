#include "Device.hpp"

Device::Device(VkPhysicalDevice physical, Vulkan *instance, QueueManager* manager) {
	m_physical = physical;
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	
	float queuePriority = 1.0f;
	for (uint32_t i = 0; i < manager->m_count.size(); ++i) {
		if (!manager->m_count[i])
			continue;
		VkDeviceQueueCreateInfo info_q{};
		info_q.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		info_q.queueFamilyIndex = i;
		info_q.queueCount = manager->m_count[i];
		info_q.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(info_q);
	}

	VkPhysicalDeviceFeatures features{};
	features.samplerAnisotropy = VK_TRUE;

	VkDeviceCreateInfo info_c{};
	info_c.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	info_c.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	info_c.pQueueCreateInfos = queueCreateInfos.data();

	info_c.pEnabledFeatures = &features;

	info_c.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	info_c.ppEnabledExtensionNames = deviceExtensions.data();

	
	if (instance->validates()) {
		info_c.enabledLayerCount = static_cast<uint32_t>(instance->m_layers.size());
		info_c.ppEnabledLayerNames = instance->m_layers.data();
	} else {
		info_c.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physical, &info_c, nullptr, &m_device) != VK_SUCCESS)
		throw std::runtime_error("failed to create logical device!");

	for (int i = 0; i < 4; ++i) {
		Queue *q = manager->getQueue(i);
		if (!q->m_queue)
			vkGetDeviceQueue(m_device, q->m_family, q->m_index, &q->m_queue);
	}
}

Device::~Device() {
	vkDeviceWaitIdle(m_device);
	vkDestroyDevice(m_device, nullptr);
}

void Device::wait() {
	vkDeviceWaitIdle(m_device);
}