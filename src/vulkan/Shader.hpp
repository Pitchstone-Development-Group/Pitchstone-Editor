#pragma once

#include "Device.hpp"

class Shader {
public:
	Shader(Device *device, uint32_t pcsize);
	virtual ~Shader();

	virtual void draw(VkCommandBuffer *buffer, void *pushConstants, VkDescriptorSet *set, VkRect2D renderArea) = 0;
protected:
	VkShaderModule loadShaderFile(std::string fileName);

	VkPipeline m_pipeline;
	VkDescriptorSetLayout m_dslayout;
	VkPipelineLayout m_pllayout;
	uint32_t m_pcsize;
	Device *m_device;
};