#include "GraphicsShader.hpp"

GraphicsShader::GraphicsShader(Device *device, std::string shaderName) {

	VkPipelineShaderStageCreateInfo info_stages[2];
	for (int i = 0; i < 2; ++i) {

	}

	VkPipelineMultisampleStateCreateInfo info_msaa{};
	info_msaa.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	info_msaa.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;



	VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
	VkPipelineRenderingCreateInfo info_render{};
	info_render.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
	info_render.colorAttachmentCount = 1;
	info_render.pColorAttachmentFormats = &format;

	VkGraphicsPipelineCreateInfo info_gpc{};
	info_gpc.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	info_gpc.pNext = &info_render;
	info_gpc.layout = m_layout;
	info_gpc.stageCount = 2;
	info_gpc.pStages = info_stages;
	info_gpc.pMultisampleState = &info_msaa;
	info_gpc.layout = m_layout;

	vkCreateGraphicsPipelines(m_device->device(), VK_NULL_HANDLE, 1, &info_gpc, VK_NULL_HANDLE, &m_pipeline);
}

GraphicsShader::~GraphicsShader() {

}