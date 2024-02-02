#pragma once

#include "Instance.hpp"
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

	VkPhysicalDeviceMemoryProperties memory;

	std::vector<VkQueueFamilyProperties> families;
	std::vector<bool> presentation;

	std::unordered_map<VkFormat, VkFormatProperties3> formats;
};

class Device {
public:
	Device(Instance*);
	~Device();
private:
	std::vector<VkPhysicalDevice> m_physicals;
	uint32_t m_physical;

	DeviceProperties m_properties;


	VkDevice m_device;
	VkInstance m_instance;
};