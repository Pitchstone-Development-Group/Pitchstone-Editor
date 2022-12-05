#ifndef PITCHSTONE_VULKAN_FRAMEBUFFER
#define PITCHSTONE_VULKAN_FRAMEBUFFER

#include <vulkan/vulkan.h>

class Framebuffer {
public:
	Framebuffer(uint32_t width, uint32_t height);
	virtual ~Framebuffer();
private:
	VkImage m_image;
	VkSampler m_sampler;
	VkImageView m_view;
	VkFramebuffer framebuffer;
	VkDeviceMemory memory;
};

#endif
