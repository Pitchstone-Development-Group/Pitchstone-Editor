#include "GraphicsShader.hpp"

GraphicsShader::GraphicsShader(Device *device, std::string shaderName, uint32_t pcsize) : Shader(device, pcsize) {
	VkShaderModule vertex = loadShaderFile(shaderName + ".vert.spv");
	VkShaderModule fragment = loadShaderFile(shaderName + ".frag.spv");

	VkPipelineShaderStageCreateInfo info_pssc[2];
	for (int i = 0; i < 2; ++i) {
		info_pssc[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		info_pssc[i].pNext = VK_NULL_HANDLE;
		info_pssc[i].flags = 0;
		info_pssc[i].module = (i == 0) ? vertex : fragment;
		info_pssc[i].stage = (i == 0) ? VK_SHADER_STAGE_VERTEX_BIT : VK_SHADER_STAGE_FRAGMENT_BIT;
		info_pssc[i].pSpecializationInfo = VK_NULL_HANDLE;
		info_pssc[i].pName = "main";
	}

	VkVertexInputBindingDescription desc_vib[1] = {};
    desc_vib[0].stride = 32; // vertex + uv + norm
    desc_vib[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription desc_via[3] = {};
    desc_via[0].location = 0;
    desc_via[0].binding = desc_vib[0].binding;
    desc_via[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    desc_via[0].offset = 0;
    desc_via[1].location = 1;
    desc_via[1].binding = desc_vib[0].binding;
    desc_via[1].format = VK_FORMAT_R32G32_SFLOAT;
    desc_via[1].offset = 12;
    desc_via[2].location = 2;
    desc_via[2].binding = desc_vib[0].binding;
    desc_via[2].format = VK_FORMAT_R32G32B32_SFLOAT;
    desc_via[2].offset = 20;

    VkPipelineVertexInputStateCreateInfo info_visc = {};
    info_visc.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    info_visc.vertexBindingDescriptionCount = 1;
    info_visc.pVertexBindingDescriptions = desc_vib;
    info_visc.vertexAttributeDescriptionCount = 3;
    info_visc.pVertexAttributeDescriptions = desc_via;

	VkPipelineInputAssemblyStateCreateInfo info_iasc{};
	info_iasc.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	info_iasc.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	info_iasc.primitiveRestartEnable = VK_FALSE;

	VkPipelineViewportStateCreateInfo info_vsc{};
	info_vsc.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	info_vsc.viewportCount = 1;
	info_vsc.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo info_rsc{};
	info_rsc.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	info_rsc.depthClampEnable = VK_FALSE;
	info_rsc.rasterizerDiscardEnable = VK_FALSE;
	info_rsc.polygonMode = VK_POLYGON_MODE_FILL;
	info_rsc.lineWidth = 1.0f;
	info_rsc.cullMode = VK_CULL_MODE_BACK_BIT;
	info_rsc.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	info_rsc.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo info_msaac{};
	info_msaac.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	info_msaac.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState state_cba{};
	state_cba.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	state_cba.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo info_cbsc{};
	info_cbsc.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	info_cbsc.logicOpEnable = VK_FALSE;
	info_cbsc.logicOp = VK_LOGIC_OP_COPY;
	info_cbsc.attachmentCount = 1;
	info_cbsc.pAttachments = &state_cba;
	info_cbsc.blendConstants[0] = 0.0f;
	info_cbsc.blendConstants[1] = 0.0f;
	info_cbsc.blendConstants[2] = 0.0f;
	info_cbsc.blendConstants[3] = 0.0f;

	std::vector<VkDynamicState> dynamic_states = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	VkPipelineDynamicStateCreateInfo info_dsc{};
	info_dsc.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	info_dsc.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
	info_dsc.pDynamicStates = dynamic_states.data();

	VkDescriptorSetLayoutBinding bindings[2];
	bindings[0].binding = 0;
	bindings[1].binding = 1;
	bindings[0].descriptorCount = 1;
	bindings[1].descriptorCount = 1;
	bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	bindings[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

	VkDescriptorSetLayoutCreateInfo info_dslc{};
	info_dslc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	info_dslc.bindingCount = 2;
	info_dslc.pBindings = bindings;
	vkCreateDescriptorSetLayout(m_device->device(), &info_dslc, VK_NULL_HANDLE, &m_dslayout);

	VkPushConstantRange pcrange{};
	pcrange.offset = 0;
	pcrange.size = m_pcsize;
	pcrange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

	VkPipelineLayoutCreateInfo info_plc{};
	info_plc.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	info_plc.setLayoutCount = 1;
	info_plc.pSetLayouts = &m_dslayout;
	if (pcsize > 0) {
		info_plc.pushConstantRangeCount = 1;
		info_plc.pPushConstantRanges = &pcrange;
	}

	VkFormat format = VK_FORMAT_R16G16B16A16_SFLOAT;
	VkPipelineRenderingCreateInfo info_render{};
	info_render.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
	info_render.colorAttachmentCount = 1;
	info_render.pColorAttachmentFormats = &format;

	VkGraphicsPipelineCreateInfo info_gpc{};
	info_gpc.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	info_gpc.pNext = &info_render;
	info_gpc.flags = 0;
	info_gpc.stageCount = 2;
	info_gpc.pStages = info_pssc;
	info_gpc.pVertexInputState = &info_visc;
	info_gpc.pInputAssemblyState = &info_iasc;
	info_gpc.pTessellationState = VK_NULL_HANDLE;
	info_gpc.pViewportState = &info_vsc;
	info_gpc.pRasterizationState = &info_rsc;
	info_gpc.pMultisampleState = &info_msaac;
	info_gpc.pDepthStencilState = VK_NULL_HANDLE;
	info_gpc.pColorBlendState = &info_cbsc;
	info_gpc.pDynamicState = &info_dsc;
	info_gpc.layout = m_pllayout;
	info_gpc.renderPass = VK_NULL_HANDLE;
	info_gpc.subpass = 0;
	info_gpc.basePipelineHandle = VK_NULL_HANDLE;
	info_gpc.basePipelineIndex = 0;

	vkCreateGraphicsPipelines(m_device->device(), VK_NULL_HANDLE, 1, &info_gpc, VK_NULL_HANDLE, &m_pipeline);
}

GraphicsShader::~GraphicsShader() {

}