#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

class Instance {
public:
	Instance();
	~Instance();

	VkInstance instance() { return m_instance; }
	const std::vector<std::string>& extensions() { return m_extensions; }
private:
	VkInstance m_instance;
	std::vector<std::string> m_extensions;
};