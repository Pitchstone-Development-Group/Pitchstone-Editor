#include "Device.hpp"

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

Device::Device(Instance *instance) {
	m_device = VK_NULL_HANDLE;
	m_instance = instance->instance();
	uint32_t count;
	vkEnumeratePhysicalDevices(m_instance, &count, VK_NULL_HANDLE);
	m_physicals.resize(count);
	vkEnumeratePhysicalDevices(m_instance, &count, m_physicals.data());

	m_physical = 0;
	
	float priorities[1] = { 1.0F };

	VkDeviceQueueCreateInfo info_que{};
	info_que.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	info_que.pNext = VK_NULL_HANDLE;
	info_que.flags = 0;
	info_que.queueFamilyIndex = 0;
	info_que.queueCount = 1;
	info_que.pQueuePriorities = priorities;

	VkDeviceCreateInfo info_dev{};
	info_dev.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	info_dev.pNext = &m_properties.features_11;
	info_dev.flags = 0;
	info_dev.queueCreateInfoCount = 1;
	info_dev.pQueueCreateInfos = &info_que;
	info_dev.enabledLayerCount = 0;
	info_dev.ppEnabledLayerNames = VK_NULL_HANDLE;
	info_dev.enabledExtensionCount = 0;
	info_dev.ppEnabledExtensionNames = VK_NULL_HANDLE;
	info_dev.pEnabledFeatures = &m_properties.features_10.features;

	vkCreateDevice(m_physicals[m_physical], &info_dev, VK_NULL_HANDLE, &m_device);
}

Device::~Device() {
	vkDestroyDevice(m_device, VK_NULL_HANDLE);
}