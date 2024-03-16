#include "ImGui_13.hpp"
#include <algorithm>
#include <iostream>

/* Directly copied from imgui_impl_vulkan.cpp */
//-----------------------------------------------------------------------------
// SHADERS
//-----------------------------------------------------------------------------

// backends/vulkan/glsl_shader.vert, compiled with:
// # glslangValidator -V -x -o glsl_shader.vert.u32 glsl_shader.vert
/*
#version 450 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec4 aColor;
layout(push_constant) uniform uPushConstant { vec2 uScale; vec2 uTranslate; } pc;

out gl_PerVertex { vec4 gl_Position; };
layout(location = 0) out struct { vec4 Color; vec2 UV; } Out;

void main()
{
	Out.Color = aColor;
	Out.UV = aUV;
	gl_Position = vec4(aPos * pc.uScale + pc.uTranslate, 0, 1);
}
*/
static uint32_t __glsl_shader_vert_spv[] =
{
	0x07230203,0x00010000,0x00080001,0x0000002e,0x00000000,0x00020011,0x00000001,0x0006000b,
	0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
	0x000a000f,0x00000000,0x00000004,0x6e69616d,0x00000000,0x0000000b,0x0000000f,0x00000015,
	0x0000001b,0x0000001c,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
	0x00000000,0x00030005,0x00000009,0x00000000,0x00050006,0x00000009,0x00000000,0x6f6c6f43,
	0x00000072,0x00040006,0x00000009,0x00000001,0x00005655,0x00030005,0x0000000b,0x0074754f,
	0x00040005,0x0000000f,0x6c6f4361,0x0000726f,0x00030005,0x00000015,0x00565561,0x00060005,
	0x00000019,0x505f6c67,0x65567265,0x78657472,0x00000000,0x00060006,0x00000019,0x00000000,
	0x505f6c67,0x7469736f,0x006e6f69,0x00030005,0x0000001b,0x00000000,0x00040005,0x0000001c,
	0x736f5061,0x00000000,0x00060005,0x0000001e,0x73755075,0x6e6f4368,0x6e617473,0x00000074,
	0x00050006,0x0000001e,0x00000000,0x61635375,0x0000656c,0x00060006,0x0000001e,0x00000001,
	0x61725475,0x616c736e,0x00006574,0x00030005,0x00000020,0x00006370,0x00040047,0x0000000b,
	0x0000001e,0x00000000,0x00040047,0x0000000f,0x0000001e,0x00000002,0x00040047,0x00000015,
	0x0000001e,0x00000001,0x00050048,0x00000019,0x00000000,0x0000000b,0x00000000,0x00030047,
	0x00000019,0x00000002,0x00040047,0x0000001c,0x0000001e,0x00000000,0x00050048,0x0000001e,
	0x00000000,0x00000023,0x00000000,0x00050048,0x0000001e,0x00000001,0x00000023,0x00000008,
	0x00030047,0x0000001e,0x00000002,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,
	0x00030016,0x00000006,0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040017,
	0x00000008,0x00000006,0x00000002,0x0004001e,0x00000009,0x00000007,0x00000008,0x00040020,
	0x0000000a,0x00000003,0x00000009,0x0004003b,0x0000000a,0x0000000b,0x00000003,0x00040015,
	0x0000000c,0x00000020,0x00000001,0x0004002b,0x0000000c,0x0000000d,0x00000000,0x00040020,
	0x0000000e,0x00000001,0x00000007,0x0004003b,0x0000000e,0x0000000f,0x00000001,0x00040020,
	0x00000011,0x00000003,0x00000007,0x0004002b,0x0000000c,0x00000013,0x00000001,0x00040020,
	0x00000014,0x00000001,0x00000008,0x0004003b,0x00000014,0x00000015,0x00000001,0x00040020,
	0x00000017,0x00000003,0x00000008,0x0003001e,0x00000019,0x00000007,0x00040020,0x0000001a,
	0x00000003,0x00000019,0x0004003b,0x0000001a,0x0000001b,0x00000003,0x0004003b,0x00000014,
	0x0000001c,0x00000001,0x0004001e,0x0000001e,0x00000008,0x00000008,0x00040020,0x0000001f,
	0x00000009,0x0000001e,0x0004003b,0x0000001f,0x00000020,0x00000009,0x00040020,0x00000021,
	0x00000009,0x00000008,0x0004002b,0x00000006,0x00000028,0x00000000,0x0004002b,0x00000006,
	0x00000029,0x3f800000,0x00050036,0x00000002,0x00000004,0x00000000,0x00000003,0x000200f8,
	0x00000005,0x0004003d,0x00000007,0x00000010,0x0000000f,0x00050041,0x00000011,0x00000012,
	0x0000000b,0x0000000d,0x0003003e,0x00000012,0x00000010,0x0004003d,0x00000008,0x00000016,
	0x00000015,0x00050041,0x00000017,0x00000018,0x0000000b,0x00000013,0x0003003e,0x00000018,
	0x00000016,0x0004003d,0x00000008,0x0000001d,0x0000001c,0x00050041,0x00000021,0x00000022,
	0x00000020,0x0000000d,0x0004003d,0x00000008,0x00000023,0x00000022,0x00050085,0x00000008,
	0x00000024,0x0000001d,0x00000023,0x00050041,0x00000021,0x00000025,0x00000020,0x00000013,
	0x0004003d,0x00000008,0x00000026,0x00000025,0x00050081,0x00000008,0x00000027,0x00000024,
	0x00000026,0x00050051,0x00000006,0x0000002a,0x00000027,0x00000000,0x00050051,0x00000006,
	0x0000002b,0x00000027,0x00000001,0x00070050,0x00000007,0x0000002c,0x0000002a,0x0000002b,
	0x00000028,0x00000029,0x00050041,0x00000011,0x0000002d,0x0000001b,0x0000000d,0x0003003e,
	0x0000002d,0x0000002c,0x000100fd,0x00010038
};

// backends/vulkan/glsl_shader.frag, compiled with:
// # glslangValidator -V -x -o glsl_shader.frag.u32 glsl_shader.frag
/*
#version 450 core
layout(location = 0) out vec4 fColor;
layout(set=0, binding=0) uniform sampler2D sTexture;
layout(location = 0) in struct { vec4 Color; vec2 UV; } In;
void main()
{
	fColor = In.Color * texture(sTexture, In.UV.st);
}
*/
static uint32_t __glsl_shader_frag_spv[] =
{
	0x07230203,0x00010000,0x00080001,0x0000001e,0x00000000,0x00020011,0x00000001,0x0006000b,
	0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
	0x0007000f,0x00000004,0x00000004,0x6e69616d,0x00000000,0x00000009,0x0000000d,0x00030010,
	0x00000004,0x00000007,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
	0x00000000,0x00040005,0x00000009,0x6c6f4366,0x0000726f,0x00030005,0x0000000b,0x00000000,
	0x00050006,0x0000000b,0x00000000,0x6f6c6f43,0x00000072,0x00040006,0x0000000b,0x00000001,
	0x00005655,0x00030005,0x0000000d,0x00006e49,0x00050005,0x00000016,0x78655473,0x65727574,
	0x00000000,0x00040047,0x00000009,0x0000001e,0x00000000,0x00040047,0x0000000d,0x0000001e,
	0x00000000,0x00040047,0x00000016,0x00000022,0x00000000,0x00040047,0x00000016,0x00000021,
	0x00000000,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,0x00030016,0x00000006,
	0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040020,0x00000008,0x00000003,
	0x00000007,0x0004003b,0x00000008,0x00000009,0x00000003,0x00040017,0x0000000a,0x00000006,
	0x00000002,0x0004001e,0x0000000b,0x00000007,0x0000000a,0x00040020,0x0000000c,0x00000001,
	0x0000000b,0x0004003b,0x0000000c,0x0000000d,0x00000001,0x00040015,0x0000000e,0x00000020,
	0x00000001,0x0004002b,0x0000000e,0x0000000f,0x00000000,0x00040020,0x00000010,0x00000001,
	0x00000007,0x00090019,0x00000013,0x00000006,0x00000001,0x00000000,0x00000000,0x00000000,
	0x00000001,0x00000000,0x0003001b,0x00000014,0x00000013,0x00040020,0x00000015,0x00000000,
	0x00000014,0x0004003b,0x00000015,0x00000016,0x00000000,0x0004002b,0x0000000e,0x00000018,
	0x00000001,0x00040020,0x00000019,0x00000001,0x0000000a,0x00050036,0x00000002,0x00000004,
	0x00000000,0x00000003,0x000200f8,0x00000005,0x00050041,0x00000010,0x00000011,0x0000000d,
	0x0000000f,0x0004003d,0x00000007,0x00000012,0x00000011,0x0004003d,0x00000014,0x00000017,
	0x00000016,0x00050041,0x00000019,0x0000001a,0x0000000d,0x00000018,0x0004003d,0x0000000a,
	0x0000001b,0x0000001a,0x00050057,0x00000007,0x0000001c,0x00000017,0x0000001b,0x00050085,
	0x00000007,0x0000001d,0x00000012,0x0000001c,0x0003003e,0x00000009,0x0000001d,0x000100fd,
	0x00010038
};

ImGui_13::ImGui_13(Device *device, VkSurfaceKHR surface, uint32_t width, uint32_t height) {
	ImGuiIO& io = ImGui::GetIO();
	io.BackendRendererUserData = (void*)this;
	io.BackendRendererName = "ImGui_13";
	io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

	m_device = device;
	m_queueWindow = m_device->queue(QUEUE_WINDOW_GRAPHICS);
	m_queuePresent = m_device->queue(QUEUE_WINDOW_PRESENT);
	m_window.surface = surface;

	createPipeline();
	createFonts();

	m_window.swapchain = VK_NULL_HANDLE;
	createOrResizeSwapchain(width, height);
	createImages(width, height);
	createSyncs();
}

ImGui_13::~ImGui_13() {
	vkDeviceWaitIdle(m_device->device());
	destroyImages();
	destroyFonts();
	destroyPipeline();
	destroySyncs();
	destroySwapchain();
	auto& io = ImGui::GetIO();
	io.BackendRendererUserData = nullptr;
	io.BackendPlatformUserData = nullptr;
}

VkDescriptorSet ImGui_13::add(VkSampler sampler, VkImageView view, VkImageLayout layout) {
	VkDescriptorSet descriptor_set;
	VkResult result;

	VkDescriptorSetAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.descriptorPool = m_pool;
	alloc_info.descriptorSetCount = 1;
	alloc_info.pSetLayouts = &m_dslayout;
	result = vkAllocateDescriptorSets(m_device->device(), &alloc_info, &descriptor_set);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("ImGui_13 ERROR: Not able to allocate descriptor set for texture!");
	}

	VkDescriptorImageInfo desc_image[1] = {};
	desc_image[0].sampler = sampler;
	desc_image[0].imageView = view;
	desc_image[0].imageLayout = layout;
	VkWriteDescriptorSet write_desc[1] = {};
	write_desc[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write_desc[0].dstSet = descriptor_set;
	write_desc[0].descriptorCount = 1;
	write_desc[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	write_desc[0].pImageInfo = desc_image;
	vkUpdateDescriptorSets(m_device->device(), 1, write_desc, 0, nullptr);

	return descriptor_set;
}

void ImGui_13::remove(VkDescriptorSet set) {
	vkFreeDescriptorSets(m_device->device(), m_pool, 1, &set);
}

void ImGui_13::createPipeline() {
	VkResult result;

	VkShaderModule moduleVert, moduleFrag;

	VkShaderModuleCreateInfo vert_info = {};
	vert_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vert_info.codeSize = sizeof(__glsl_shader_vert_spv);
	vert_info.pCode = (uint32_t*)__glsl_shader_vert_spv;
	result = vkCreateShaderModule(m_device->device(), &vert_info, VK_NULL_HANDLE, &moduleVert);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("ImGui_13 ERROR: Not able to create vertex shader module!");
	}

	VkShaderModuleCreateInfo frag_info = {};
	frag_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	frag_info.codeSize = sizeof(__glsl_shader_frag_spv);
	frag_info.pCode = (uint32_t*)__glsl_shader_frag_spv;
	result = vkCreateShaderModule(m_device->device(), &frag_info, VK_NULL_HANDLE, &moduleFrag);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("ImGui_13 ERROR: Not able to create fragment shader module!");
	}

	VkDescriptorSetLayoutBinding binding[1] = {};
	binding[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	binding[0].descriptorCount = 1;
	binding[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	VkDescriptorSetLayoutCreateInfo dslinfo = {};
	dslinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	dslinfo.bindingCount = 1;
	dslinfo.pBindings = binding;
	result = vkCreateDescriptorSetLayout(m_device->device(), &dslinfo, VK_NULL_HANDLE, &m_dslayout);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("ImGui_13 ERROR: Not able to create descriptor set layout!");
	}

	VkPushConstantRange push_constants[1] = {};
	push_constants[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	push_constants[0].offset = sizeof(float) * 0;
	push_constants[0].size = sizeof(float) * 4;
	VkDescriptorSetLayout set_layout[1] = { m_dslayout };
	VkPipelineLayoutCreateInfo layout_info = {};
	layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layout_info.setLayoutCount = 1;
	layout_info.pSetLayouts = set_layout;
	layout_info.pushConstantRangeCount = 1;
	layout_info.pPushConstantRanges = push_constants;
	result = vkCreatePipelineLayout(m_device->device(), &layout_info, VK_NULL_HANDLE, &m_pllayout);

	VkPipelineShaderStageCreateInfo stage[2] = {};
	stage[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stage[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	stage[0].module = moduleVert;
	stage[0].pName = "main";
	stage[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stage[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	stage[1].module = moduleFrag;
	stage[1].pName = "main";

	VkVertexInputBindingDescription binding_desc[1] = {};
	binding_desc[0].stride = sizeof(ImDrawVert);
	binding_desc[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputAttributeDescription attribute_desc[3] = {};
	attribute_desc[0].location = 0;
	attribute_desc[0].binding = binding_desc[0].binding;
	attribute_desc[0].format = VK_FORMAT_R32G32_SFLOAT;
	attribute_desc[0].offset = offsetof(ImDrawVert, pos);
	attribute_desc[1].location = 1;
	attribute_desc[1].binding = binding_desc[0].binding;
	attribute_desc[1].format = VK_FORMAT_R32G32_SFLOAT;
	attribute_desc[1].offset = offsetof(ImDrawVert, uv);
	attribute_desc[2].location = 2;
	attribute_desc[2].binding = binding_desc[0].binding;
	attribute_desc[2].format = VK_FORMAT_R8G8B8A8_UNORM;
	attribute_desc[2].offset = offsetof(ImDrawVert, col);

	VkPipelineVertexInputStateCreateInfo vertex_info = {};
	vertex_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_info.vertexBindingDescriptionCount = 1;
	vertex_info.pVertexBindingDescriptions = binding_desc;
	vertex_info.vertexAttributeDescriptionCount = 3;
	vertex_info.pVertexAttributeDescriptions = attribute_desc;

	VkPipelineInputAssemblyStateCreateInfo ia_info = {};
	ia_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	ia_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	VkPipelineViewportStateCreateInfo viewport_info = {};
	viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_info.viewportCount = 1;
	viewport_info.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo raster_info = {};
	raster_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	raster_info.polygonMode = VK_POLYGON_MODE_FILL;
	raster_info.cullMode = VK_CULL_MODE_NONE;
	raster_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	raster_info.lineWidth = 1.0f;

	VkPipelineMultisampleStateCreateInfo ms_info = {};
	ms_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	ms_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState color_attachment[1] = {};
	color_attachment[0].blendEnable = VK_TRUE;
	color_attachment[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	color_attachment[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	color_attachment[0].colorBlendOp = VK_BLEND_OP_ADD;
	color_attachment[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	color_attachment[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	color_attachment[0].alphaBlendOp = VK_BLEND_OP_ADD;
	color_attachment[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineDepthStencilStateCreateInfo depth_info = {};
	depth_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

	VkPipelineColorBlendStateCreateInfo blend_info = {};
	blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	blend_info.attachmentCount = 1;
	blend_info.pAttachments = color_attachment;

	VkDynamicState dynamic_states[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	VkPipelineDynamicStateCreateInfo dynamic_state = {};
	dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state.dynamicStateCount = (uint32_t)IM_ARRAYSIZE(dynamic_states);
	dynamic_state.pDynamicStates = dynamic_states;

	VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
	VkPipelineRenderingCreateInfoKHR pipelineRenderingCreateInfo = {};
	pipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
	pipelineRenderingCreateInfo.colorAttachmentCount = 1;
	pipelineRenderingCreateInfo.pColorAttachmentFormats = &format;

	VkGraphicsPipelineCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	info.pNext = &pipelineRenderingCreateInfo;
	info.flags = 0;
	info.stageCount = 2;
	info.pStages = stage;
	info.pVertexInputState = &vertex_info;
	info.pInputAssemblyState = &ia_info;
	info.pViewportState = &viewport_info;
	info.pRasterizationState = &raster_info;
	info.pMultisampleState = &ms_info;
	info.pDepthStencilState = &depth_info;
	info.pColorBlendState = &blend_info;
	info.pDynamicState = &dynamic_state;
	info.layout = m_pllayout;
	info.renderPass = VK_NULL_HANDLE;
	info.subpass = 0;

	result = vkCreateGraphicsPipelines(m_device->device(), VK_NULL_HANDLE, 1, &info, VK_NULL_HANDLE, &m_pipeline);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("ImGui_13 ERROR: Not able to create fragment shader module!");
	}

	vkDestroyShaderModule(m_device->device(), moduleVert, VK_NULL_HANDLE);
	vkDestroyShaderModule(m_device->device(), moduleFrag, VK_NULL_HANDLE);

	// Will need to tweak over time
	uint32_t maxSizePerType = 128;
	std::vector<VkDescriptorPoolSize> poolSizes =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, maxSizePerType },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, maxSizePerType },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, maxSizePerType }
	};

	VkDescriptorPoolCreateInfo infoPool = {};
	infoPool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	infoPool.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	infoPool.maxSets = maxSizePerType * (uint32_t) poolSizes.size();
	infoPool.poolSizeCount = (uint32_t) poolSizes.size();
	infoPool.pPoolSizes = poolSizes.data();
	vkCreateDescriptorPool(m_device->device(), &infoPool, VK_NULL_HANDLE, &m_pool);
}

void ImGui_13::destroyPipeline() {
	vkDestroyPipeline(m_device->device(), m_pipeline, VK_NULL_HANDLE);
	vkDestroyPipelineLayout(m_device->device(), m_pllayout, VK_NULL_HANDLE);
	vkDestroyDescriptorSetLayout(m_device->device(), m_dslayout, VK_NULL_HANDLE);
	vkDestroyDescriptorPool(m_device->device(), m_pool, VK_NULL_HANDLE);
}

void ImGui_13::createFonts() {
	ImGuiIO& io = ImGui::GetIO();
	VkResult result;

	VkSamplerCreateInfo sampler = {};
	sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler.magFilter = VK_FILTER_LINEAR;
	sampler.minFilter = VK_FILTER_LINEAR;
	sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler.minLod = 0;
	sampler.maxLod = VK_LOD_CLAMP_NONE;
	sampler.maxAnisotropy = 1.0f;
	result = vkCreateSampler(m_device->device(), &sampler, VK_NULL_HANDLE, &m_font.sampler);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("ImGui_13 ERROR: Not able to create sampler for fonts!");
	}

	VkCommandPoolCreateInfo poolinfo = {};
	poolinfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolinfo.flags = 0;
	poolinfo.queueFamilyIndex = m_queueWindow->family();

	CommandPool *pool = new CommandPool(m_device, 1, m_queueWindow->family());
	
	pool->reset();

	VkCommandBuffer cbuffer = pool->buffer(0);
	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	result = vkBeginCommandBuffer(cbuffer, &begin_info);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("ImGui_13 ERROR: Not able to begin command buffer!");
	}

	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
	size_t upload_size = width * height * 4 * sizeof(char);

	m_font.image = new Image(m_device, (uint32_t)width, (uint32_t)height, VK_FORMAT_R8G8B8A8_UNORM);

	// Create the Descriptor Set:
	m_font.set = add(m_font.sampler, m_font.image->view(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	std::vector<VkMemoryPropertyFlags> choices = { VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT };
	Buffer *upload = new Buffer(m_device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, std::max(upload_size, 1048576ULL), choices, true);

	void* map = upload->map();
	memcpy(map, pixels, upload_size);
	upload->unmap();

	VkImageMemoryBarrier copy_barrier[1] = {};
	copy_barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	copy_barrier[0].dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	copy_barrier[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	copy_barrier[0].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	copy_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	copy_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	copy_barrier[0].image = m_font.image->image();
	copy_barrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	copy_barrier[0].subresourceRange.levelCount = 1;
	copy_barrier[0].subresourceRange.layerCount = 1;
	vkCmdPipelineBarrier(cbuffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, copy_barrier);

	VkBufferImageCopy region = {};
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.layerCount = 1;
	region.imageExtent.width = width;
	region.imageExtent.height = height;
	region.imageExtent.depth = 1;
	vkCmdCopyBufferToImage(cbuffer, upload->buffer(), m_font.image->image(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	VkImageMemoryBarrier use_barrier[1] = {};
	use_barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	use_barrier[0].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	use_barrier[0].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	use_barrier[0].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	use_barrier[0].newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	use_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	use_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	use_barrier[0].image = m_font.image->image();
	use_barrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	use_barrier[0].subresourceRange.levelCount = 1;
	use_barrier[0].subresourceRange.layerCount = 1;
	vkCmdPipelineBarrier(cbuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, use_barrier);
	
	// Store our identifier
	io.Fonts->SetTexID((ImTextureID)m_font.set);

	// End command buffer
	VkSubmitInfo end_info = {};
	end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	end_info.commandBufferCount = 1;
	end_info.pCommandBuffers = &cbuffer;
	vkEndCommandBuffer(cbuffer);

	VkFenceCreateInfo fence_info{};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	VkFence fence;
	vkCreateFence(m_device->device(), &fence_info, VK_NULL_HANDLE, &fence);

	m_queueWindow->lock();
	vkQueueSubmit(m_queueWindow->queue(), 1, &end_info, fence);
	m_queueWindow->unlock();
	vkWaitForFences(m_device->device(), 1, &fence, VK_TRUE, UINT64_MAX);

	vkDestroyFence(m_device->device(), fence, VK_NULL_HANDLE);
	delete pool;
	delete upload;
}

void ImGui_13::destroyFonts() {
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->SetTexID(0);
	vkFreeDescriptorSets(m_device->device(), m_pool, 1, &m_font.set);
	vkDestroySampler(m_device->device(), m_font.sampler, VK_NULL_HANDLE);
	delete m_font.image;
}

static VkSurfaceFormatKHR getSurfaceFormat(VkPhysicalDevice physical, VkSurfaceKHR surface) {
	uint32_t avail_count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physical, surface, &avail_count, nullptr);
	std::vector<VkSurfaceFormatKHR> avail_format(avail_count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(physical, surface, &avail_count, avail_format.data());
	
	VkFormat formats[4] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
	if (avail_count == 1) {
		if (avail_format[0].format == VK_FORMAT_UNDEFINED) {
			VkSurfaceFormatKHR ret;
			ret.format = formats[0];
			ret.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
			return ret;
		}
		else {
			return avail_format[0];
		}
	}
	else {
		for (auto f : formats)
			for (auto a : avail_format)
				if (a.format == f && a.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
					return a;
		return avail_format[0];
	}

}

void ImGui_13::createOrResizeSwapchain(uint32_t width, uint32_t height) {
	VkResult result;

	VkSwapchainKHR old_swapchain = m_window.swapchain;
	if (old_swapchain != VK_NULL_HANDLE) {
		for (auto& v : m_window.views) {
			vkDestroyImageView(m_device->device(), v, VK_NULL_HANDLE);
		}
	}

	m_window.swapchain = VK_NULL_HANDLE;
	result = vkDeviceWaitIdle(m_device->device());
	if (result != VK_SUCCESS) {
		throw std::runtime_error("ImGui_13 ERROR: Not able to wait during swapchain creation!");
	}

	m_window.format = getSurfaceFormat(m_device->physical(), m_window.surface); 

	VkSwapchainCreateInfoKHR info = {};
	info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	info.surface = m_window.surface;
	info.minImageCount = 3;
	info.imageFormat = m_window.format.format;
	info.imageColorSpace = m_window.format.colorSpace;
	info.imageArrayLayers = 1;
	info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	info.clipped = VK_TRUE;
	info.oldSwapchain = old_swapchain;

	VkSurfaceCapabilitiesKHR cap;
	result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_device->physical(), m_window.surface, &cap);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("ImGui_13 ERROR: Not able to get surface capabilities!");
	}

	if (info.minImageCount < cap.minImageCount)
		info.minImageCount = cap.minImageCount;
	else if (cap.maxImageCount != 0 && info.minImageCount > cap.maxImageCount)
		info.minImageCount = cap.maxImageCount;

	if (cap.currentExtent.width == 0xffffffff)
	{
		info.imageExtent.width = m_window.width = width;
		info.imageExtent.height = m_window.height = height;
	}
	else
	{
		info.imageExtent.width = m_window.width = cap.currentExtent.width;
		info.imageExtent.height = m_window.height = cap.currentExtent.height;
	}

	result = vkCreateSwapchainKHR(m_device->device(), &info, VK_NULL_HANDLE, &m_window.swapchain);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("ImGui_13 ERROR: Not able to create swapchain!");
	}

	uint32_t images;
	result = vkGetSwapchainImagesKHR(m_device->device(), m_window.swapchain, &images, nullptr);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("ImGui_13 ERROR: Not able to get swapchain images size!");
	}
	m_window.images.resize(images);
	result = vkGetSwapchainImagesKHR(m_device->device(), m_window.swapchain, &images, m_window.images.data());
	if (result != VK_SUCCESS) {
		throw std::runtime_error("ImGui_13 ERROR: Not able to get swapchain images!");
	}

	if (old_swapchain)
		vkDestroySwapchainKHR(m_device->device(), old_swapchain, VK_NULL_HANDLE);

	m_window.views.resize(images);
	for (uint32_t i = 0; i < images; ++i) {
		VkImageViewCreateInfo view_info{};
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.format = m_window.format.format;
		view_info.image = m_window.images[i];
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		view_info.subresourceRange.layerCount = 1;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.levelCount = 1;
		view_info.subresourceRange.baseMipLevel = 0;
		vkCreateImageView(m_device->device(), &view_info, VK_NULL_HANDLE, &m_window.views[i]);
	}
	
	m_window.rebuild = false;
}

void ImGui_13::destroySwapchain() {
	for (auto& v : m_window.views) {
		vkDestroyImageView(m_device->device(), v, VK_NULL_HANDLE);
	}
	vkDestroySwapchainKHR(m_device->device(), m_window.swapchain, VK_NULL_HANDLE);
}

void ImGui_13::createImages(uint32_t width, uint32_t height) {
	m_images[0].image = new Image(m_device, width, height, VK_FORMAT_R8G8B8A8_UNORM);
	m_images[1].image = new Image(m_device, width, height, VK_FORMAT_R8G8B8A8_UNORM);
	std::vector<VkMemoryPropertyFlags> choices = {VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT};
	m_images[0].vertices = new Buffer(m_device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 1048576, choices, false);
	m_images[1].vertices = new Buffer(m_device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 1048576, choices, false);
	m_images[0].indices = new Buffer(m_device, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 1048576, choices, false);
	m_images[1].indices = new Buffer(m_device, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 1048576, choices, false);
}

void ImGui_13::resizeImages(int index, uint32_t width, uint32_t height) {
	if (m_images[index].image) {
		delete m_images[index].image;
		m_images[index].image = nullptr;
	}
	if (width == 0 || height == 0) {
		return;
	}
	m_images[index].image = new Image(m_device, width, height, VK_FORMAT_R8G8B8A8_UNORM);	
}

void ImGui_13::destroyImages() {
	delete m_images[0].image;
	delete m_images[1].image;
	
	delete m_images[0].vertices;
	delete m_images[1].vertices;
	delete m_images[0].indices;
	delete m_images[1].indices;
}

void ImGui_13::createSyncs() {
	m_index = 1;
	m_timeline.waitDraw = 0;
	m_timeline.waitBlit = 1;
	m_timeline.waitPresent = 2;

	m_window.fif = new FiFSyncs(m_device, m_queueWindow->family());

	m_timeline.semaphore = new Timeline(m_device);

	m_images[0].pool = new CommandPool(m_device, 1, m_queueWindow->family());
	m_images[1].pool = new CommandPool(m_device, 1, m_queueWindow->family());
}

void ImGui_13::destroySyncs() {
	delete m_timeline.semaphore;
	delete m_images[0].pool;
	delete m_images[1].pool;
	delete m_window.fif;
}

void ImGui_13::draw(ImDrawData *data) {
	uint32_t fb_width = (uint32_t)(data->DisplaySize.x * data->FramebufferScale.x);
    uint32_t fb_height = (uint32_t)(data->DisplaySize.y * data->FramebufferScale.y);
    if (fb_width <= 0 || fb_height <= 0)
        return;

	m_timeline.semaphore->wait(m_timeline.waitDraw, UINT64_MAX);
	m_index ^= 1;
	if (m_images[m_index].image->width() != (uint32_t)fb_width || m_images[m_index].image->height() != (uint32_t)fb_height) {
		resizeImages(m_index, (uint32_t)fb_width, (uint32_t)fb_height);
	}

	if (data->TotalVtxCount > 0) {
		std::vector<VkMemoryPropertyFlags> choices = {VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT};
		VkDeviceSize vtx_size = ((data->TotalVtxCount * sizeof(ImDrawVert) + 1048575) >> 20) << 20;
		VkDeviceSize idx_size = ((data->TotalIdxCount * sizeof(ImDrawVert) + 1048575) >> 20) << 20;
		if (vtx_size > m_images[m_index].vertices->size()) {
			delete m_images[m_index].vertices;
			m_images[m_index].vertices = new Buffer(m_device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vtx_size, choices, false);
		}
		if (idx_size > m_images[m_index].indices->size()) {
			delete m_images[m_index].indices;
			m_images[m_index].vertices = new Buffer(m_device, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, idx_size, choices, false);
		}


		ImDrawVert* vtx_dst = (ImDrawVert*) m_images[m_index].vertices->map();
		for (int i = 0; i < data->CmdListsCount; ++i) {
			const ImDrawList* cmd_list = data->CmdLists[i];
			memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
			vtx_dst += cmd_list->VtxBuffer.Size;
		}
		m_images[m_index].vertices->unmap();

		ImDrawIdx* idx_dst = (ImDrawIdx*) m_images[m_index].indices->map();
		for (int i = 0; i < data->CmdListsCount; ++i) {
			const ImDrawList* cmd_list = data->CmdLists[i];
			memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
			idx_dst += cmd_list->IdxBuffer.Size;
		}
		m_images[m_index].indices->unmap();
	}

	m_images[m_index].pool->reset();
	VkCommandBuffer buffer = m_images[m_index].pool->buffer(0);

	VkCommandBufferBeginInfo beginner{};
	beginner.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginner.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(buffer, &beginner);

	m_images[m_index].image->barrier(	buffer, 0, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
										VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
										VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);

	VkRenderingAttachmentInfo attachment{};
	attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	attachment.clearValue.color.float32[0] = 0.1F;
	attachment.clearValue.color.float32[1] = 0.1F;
	attachment.clearValue.color.float32[2] = 0.1F;
	attachment.clearValue.color.float32[3] = 1.0F;
	attachment.imageView = m_images[m_index].image->view();
	attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachment.resolveMode = VK_RESOLVE_MODE_NONE;
	attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	VkRenderingInfo rendering{};
	rendering.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	rendering.colorAttachmentCount = 1;
	rendering.pColorAttachments = &attachment;
	rendering.layerCount = 1;
	rendering.renderArea.offset.x = 0;
	rendering.renderArea.offset.y = 0;
	rendering.renderArea.extent.width = fb_width;
	rendering.renderArea.extent.height = fb_height;
	vkCmdBeginRendering(buffer, &rendering);

	if (data->TotalVtxCount > 0) {
		vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

		VkBuffer vertex_buffers[1] = { m_images[m_index].vertices->buffer() };
		VkDeviceSize vertex_offset[1] = { 0 };
		vkCmdBindVertexBuffers(buffer, 0, 1, vertex_buffers, vertex_offset);
		vkCmdBindIndexBuffer(buffer, m_images[m_index].indices->buffer(), 0, sizeof(ImDrawIdx) == 2 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);

		VkViewport viewport;
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = (float)fb_width;
		viewport.height = (float)fb_height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(buffer, 0, 1, &viewport);

		float scale[2];
		scale[0] = 2.0f / data->DisplaySize.x;
		scale[1] = 2.0f / data->DisplaySize.y;
		float translate[2];
		translate[0] = -1.0f - data->DisplayPos.x * scale[0];
		translate[1] = -1.0f - data->DisplayPos.y * scale[1];
		
		float transform[4] = {scale[0], scale[1], translate[0], translate[1]};
		vkCmdPushConstants(buffer, m_pllayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * 4, transform);

		ImVec2 clip_off = data->DisplayPos;         // (0,0) unless using multi-viewports
		ImVec2 clip_scale = data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

		// Render command lists
		// (Because we merged all buffers into a single one, we maintain our own offset into them)
		int global_vtx_offset = 0;
		int global_idx_offset = 0;
		for (int n = 0; n < data->CmdListsCount; n++)
		{
			const ImDrawList* cmd_list = data->CmdLists[n];
			for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
			{
				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
				// Project scissor/clipping rectangles into framebuffer space
				ImVec2 clip_min((pcmd->ClipRect.x - clip_off.x) * clip_scale.x, (pcmd->ClipRect.y - clip_off.y) * clip_scale.y);
				ImVec2 clip_max((pcmd->ClipRect.z - clip_off.x) * clip_scale.x, (pcmd->ClipRect.w - clip_off.y) * clip_scale.y);

				// Clamp to viewport as vkCmdSetScissor() won't accept values that are off bounds
				if (clip_min.x < 0.0f) { clip_min.x = 0.0f; }
				if (clip_min.y < 0.0f) { clip_min.y = 0.0f; }
				if (clip_max.x > fb_width) { clip_max.x = (float)fb_width; }
				if (clip_max.y > fb_height) { clip_max.y = (float)fb_height; }
				if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
					continue;

				// Apply scissor/clipping rectangle
				VkRect2D scissor;
				scissor.offset.x = (int32_t)(clip_min.x);
				scissor.offset.y = (int32_t)(clip_min.y);
				scissor.extent.width = (uint32_t)(clip_max.x - clip_min.x);
				scissor.extent.height = (uint32_t)(clip_max.y - clip_min.y);
				vkCmdSetScissor(buffer, 0, 1, &scissor);

				// Bind DescriptorSet with font or user texture
				VkDescriptorSet desc_set[1] = { (VkDescriptorSet)pcmd->TextureId };
				vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pllayout, 0, 1, desc_set, 0, nullptr);

				// Draw
				vkCmdDrawIndexed(buffer, pcmd->ElemCount, 1, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset, 0);
			}
			global_idx_offset += cmd_list->IdxBuffer.Size;
			global_vtx_offset += cmd_list->VtxBuffer.Size;
		}
	}

	vkCmdEndRendering(buffer);

	m_images[m_index].image->barrier(	buffer, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_2_TRANSFER_READ_BIT,
										VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
										VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_2_BLIT_BIT);

	vkEndCommandBuffer(buffer);

	VkSemaphoreSubmitInfo waiting{};
	waiting.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	waiting.deviceIndex = 0;
	waiting.semaphore = m_timeline.semaphore->semaphore();
	waiting.value = m_timeline.waitDraw;
	waiting.stageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;

	VkSemaphoreSubmitInfo signaling{};
	signaling.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	signaling.deviceIndex = 0;
	signaling.semaphore = m_timeline.semaphore->semaphore();
	signaling.value = m_timeline.waitBlit;
	signaling.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

	VkCommandBufferSubmitInfo buffering{};
	buffering.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	buffering.deviceMask = 0;
	buffering.commandBuffer = buffer;

	VkSubmitInfo2 submit{};
	submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	submit.commandBufferInfoCount = 1;
	submit.pCommandBufferInfos = &buffering;
	submit.waitSemaphoreInfoCount = 1;
	submit.pWaitSemaphoreInfos = &waiting;
	submit.signalSemaphoreInfoCount = 1;
	submit.pSignalSemaphoreInfos = &signaling;

	m_queueWindow->lock();
	vkQueueSubmit2(m_queueWindow->queue(), 1, &submit, VK_NULL_HANDLE);
	m_queueWindow->unlock();

	m_timeline.waitDraw += 3;
}

void ImGui_13::present(bool rebuild, uint32_t width, uint32_t height) {
	bool resize = rebuild || m_window.rebuild;
	bool current, previous;

	uint32_t srcIndex, dstIndex;

	VkResult result;
	if (resize) {
		if (width == 0 || height == 0) {
			return;
		}
		createOrResizeSwapchain(width, height);
	}

	m_window.fif->wait();

	previous = (m_timeline.waitBlit > 1);
	result = m_timeline.semaphore->wait(m_timeline.waitBlit, rebuild ? 0 : UINT64_MAX);
	current = (result == VK_SUCCESS);
	srcIndex = current ? m_index : (m_index ^ 1);
	
	result = vkAcquireNextImageKHR(m_device->device(), m_window.swapchain, UINT64_MAX, m_window.fif->acquire(), VK_NULL_HANDLE, &dstIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		m_window.rebuild = true;
		return;
	} else if (result == VK_SUBOPTIMAL_KHR) {
		m_window.rebuild = true;
	}

	m_window.fif->reset();

	VkCommandBuffer buffer = m_window.fif->buffer();

	VkCommandBufferBeginInfo begin{};
	begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(buffer, &begin);

	Image::barrier(	buffer, m_window.images[dstIndex],
					0, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
					VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
					VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);

	VkRenderingAttachmentInfo attachment{};
	attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	attachment.clearValue.color.float32[0] = 0.1F;
	attachment.clearValue.color.float32[1] = 0.1F;
	attachment.clearValue.color.float32[2] = 0.1F;
	attachment.clearValue.color.float32[3] = 1.0F;
	attachment.imageView = m_window.views[dstIndex];
	attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachment.resolveMode = VK_RESOLVE_MODE_NONE;
	attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	VkRenderingInfo rendering{};
	rendering.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	rendering.colorAttachmentCount = 1;
	rendering.pColorAttachments = &attachment;
	rendering.layerCount = 1;
	rendering.renderArea.offset.x = 0;
	rendering.renderArea.offset.y = 0;
	rendering.renderArea.extent.width = m_window.width;
	rendering.renderArea.extent.height = m_window.height;
	vkCmdBeginRendering(buffer, &rendering);

	vkCmdEndRendering(buffer);

	if (current || previous) {
		Image::barrier(	buffer, m_window.images[dstIndex],
					VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT,
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_2_BLIT_BIT);

		uint32_t w = std::min(m_window.width, m_images[srcIndex].image->width());
		uint32_t h = std::min(m_window.height, m_images[srcIndex].image->height());

		VkImageBlit2 region{};
		region.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
		region.srcOffsets[1].x = w;
		region.srcOffsets[1].y = h;
		region.srcOffsets[1].z = 1;
		region.dstOffsets[1].x = w;
		region.dstOffsets[1].y = h;
		region.dstOffsets[1].z = 1;
		region.srcSubresource.layerCount = 1;
		region.srcSubresource.baseArrayLayer = 0;
		region.srcSubresource.mipLevel = 0;
		region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.dstSubresource.layerCount = 1;
		region.dstSubresource.baseArrayLayer = 0;
		region.dstSubresource.mipLevel = 0;
		region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

		VkBlitImageInfo2 blitinfo{};
		blitinfo.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
		blitinfo.srcImage = m_images[srcIndex].image->image();
		blitinfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		blitinfo.dstImage = m_window.images[dstIndex];
		blitinfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		blitinfo.regionCount = 1;
		blitinfo.pRegions = &region;
		blitinfo.filter = VK_FILTER_NEAREST;
		vkCmdBlitImage2(buffer, &blitinfo);

		Image::barrier(	buffer, m_window.images[dstIndex],
					VK_ACCESS_2_TRANSFER_WRITE_BIT, 0,
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
					VK_PIPELINE_STAGE_2_BLIT_BIT, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT );
	} else {
		Image::barrier(	buffer, m_window.images[dstIndex],
					VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT, 0,
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
					VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);
	}

	vkEndCommandBuffer(buffer);

	VkSemaphoreSubmitInfo waiting{}, signaling[2] = {{},{}};
	waiting.sType = signaling[0].sType = signaling[1].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	waiting.deviceIndex = signaling[0].deviceIndex = signaling[1].deviceIndex = 0;
	waiting.semaphore = m_window.fif->acquire(), signaling[0].semaphore = m_window.fif->blit();
	signaling[1].semaphore = m_timeline.semaphore->semaphore();
	waiting.value = signaling[0].value = 0, signaling[1].value = m_timeline.waitPresent;
	waiting.stageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT | VK_PIPELINE_STAGE_2_BLIT_BIT | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, signaling[0].stageMask = signaling[1].stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

	VkCommandBufferSubmitInfo buffering{};
	buffering.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	buffering.deviceMask = 0;
	buffering.commandBuffer = buffer;

	VkSubmitInfo2 submit{};
	submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	submit.commandBufferInfoCount = 1;
	submit.pCommandBufferInfos = &buffering;
	submit.waitSemaphoreInfoCount = 1;
	submit.pWaitSemaphoreInfos = &waiting;
	submit.signalSemaphoreInfoCount = (current) ? 2 : 1;
	submit.pSignalSemaphoreInfos = signaling;
	
	m_queueWindow->lock();
	vkQueueSubmit2(m_queueWindow->queue(), 1, &submit, m_window.fif->fence());
	m_queueWindow->unlock();

	VkSemaphore blitsem = m_window.fif->blit();
	VkPresentInfoKHR presenting{};
	presenting.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presenting.swapchainCount = 1;
	presenting.pSwapchains = &m_window.swapchain;
	presenting.waitSemaphoreCount = 1;
	presenting.pWaitSemaphores = &blitsem;
	presenting.pImageIndices = &dstIndex;

	m_queuePresent->lock();
	result = vkQueuePresentKHR(m_queuePresent->queue(), &presenting);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		m_window.rebuild = true;
	}
	m_queuePresent->unlock();

	if (current) {
		m_timeline.semaphore->wait(m_timeline.waitPresent, UINT64_MAX);
		m_timeline.semaphore->signal(m_timeline.waitDraw);
		m_timeline.waitBlit += 3, m_timeline.waitPresent += 3;
	}

	m_window.fif->swap();
}
