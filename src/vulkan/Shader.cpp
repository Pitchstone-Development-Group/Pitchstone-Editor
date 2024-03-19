#include "Shader.hpp"
#include <iostream>
#include <fstream>

Shader::Shader(Device *device, uint32_t pcsize) {
	m_device = device;
	m_pcsize = pcsize;
}

Shader::~Shader() {
	vkDestroyPipeline(m_device->device(), m_pipeline, VK_NULL_HANDLE);
	vkDestroyPipelineLayout(m_device->device(), m_pllayout, VK_NULL_HANDLE);
	vkDestroyDescriptorSetLayout(m_device->device(), m_dslayout, VK_NULL_HANDLE);
}

VkShaderModule Shader::loadShaderFile(std::string fileName) {
	std::ifstream file(fileName, std::ios::ate | std::ios::binary);
	if (!file.is_open()) {
		throw std::runtime_error("File not found: " + fileName);
	}
	size_t fileSize = (size_t) file.tellg();
	std::vector<uint32_t> code(fileSize >> 2);
	file.seekg(0);
	file.read((char*)code.data(), fileSize);
	file.close();

	VkShaderModuleCreateInfo info_module{};
	info_module.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	info_module.pCode = code.data();
	info_module.codeSize = (uint32_t)(code.size() << 2);
	VkShaderModule shader;
	vkCreateShaderModule(m_device->device(), &info_module, VK_NULL_HANDLE, &shader);
	return shader;
}