#include "ComputeShader.hpp"

#include "ComputeShader.hpp"

ComputeShader::ComputeShader(Device *device, std::string shaderName, uint32_t pcsize) : Shader(device, pcsize) {
	VkShaderModule shader = loadShaderFile(shaderName + ".comp.spv");

	VkDescriptorSetLayoutBinding bindings[3];
	for (int i = 0; i < 3; ++i) {
		bindings[i].binding = i;
		bindings[i].descriptorCount = 1;
		bindings[i].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	}
	bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

	VkDescriptorSetLayoutCreateInfo info_dsl{};
	info_dsl.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	info_dsl.bindingCount = 3;
	info_dsl.pBindings = bindings;
	vkCreateDescriptorSetLayout(m_device->device(), &info_dsl, VK_NULL_HANDLE, &m_dslayout);

	VkPushConstantRange pcrange{};
	pcrange.offset = 0;
	pcrange.size = m_pcsize;
	pcrange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	VkPipelineLayoutCreateInfo info_plc{};
	info_plc.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	info_plc.setLayoutCount = 1;
	info_plc.pSetLayouts = &m_dslayout;
	info_plc.pushConstantRangeCount = 1;
	info_plc.pPushConstantRanges = &pcrange;
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

}