#pragma once

#include "Device.hpp"

class Shader {
public:
	Shader();
	virtual ~Shader();

	virtual void draw(VkCommandBuffer *buffer, void *pushConstants, VkDescriptorSet *set, VkRect2D renderArea) = 0;
protected:
	std::vector<uint32_t> loadShaderFile(std::string fileName);

	VkPipeline m_pipeline;
	VkDescriptorSetLayout m_dslayout;
	VkPipelineLayout m_pllayout;
	Device *m_device;
};