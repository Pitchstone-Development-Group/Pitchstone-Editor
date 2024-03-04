#include "ComputeShader.hpp"

#include "ComputeShader.hpp"

ComputeShader::ComputeShader(Device *device, std::string shaderName) {
	m_device = device;

	auto code = loadShaderFile(shaderName + ".comp");
	VkShaderModuleCreateInfo info_module{};
	info_module.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	info_module.pCode = code.data();
	info_module.codeSize = (uint32_t)(code.size() << 2);
	VkShaderModule shader;
	vkCreateShaderModule(m_device->device(), &info_module, VK_NULL_HANDLE, &shader);

	VkDescriptorSetLayoutBinding bindings[3];
	for (int i = 0; i < 3; ++i) {
		bindings[i].binding = i;
		bindings[i].descriptorCount = 1;
		bindings[i].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	}
	bindings[0].descriptorType = bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

	VkDescriptorSetLayoutCreateInfo info_dsl{};
	info_dsl.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	info_dsl.bindingCount = 3;
	info_dsl.pBindings = bindings;
	vkCreateDescriptorSetLayout(m_device->device(), &info_dsl, VK_NULL_HANDLE, &m_dslayout);

	VkPipelineLayoutCreateInfo info_plc{};
	info_plc.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	info_plc.setLayoutCount = 1;
	info_plc.pSetLayouts = &m_dslayout;
	vkCreatePipelineLayout(m_device->device(), &info_plc, VK_NULL_HANDLE, &m_pllayout);

	VkComputePipelineCreateInfo info_cpc{};
	info_cpc.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	info_cpc.layout = m_pllayout;
	info_cpc.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	info_cpc.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	info_cpc.stage.pName = "main";
	info_cpc.stage.pSpecializationInfo = VK_NULL_HANDLE;
	info_cpc.stage.module;
	vkCreateComputePipelines(m_device->device(), VK_NULL_HANDLE, 1, &info_cpc, VK_NULL_HANDLE, &m_pipeline);
}

ComputeShader::~ComputeShader() {
	vkDestroyPipeline(m_device->device(), m_pipeline, VK_NULL_HANDLE);
	vkDestroyPipelineLayout(m_device->device(), m_pllayout, VK_NULL_HANDLE);
	vkDestroyDescriptorSetLayout(m_device->device(), m_dslayout, VK_NULL_HANDLE);
}