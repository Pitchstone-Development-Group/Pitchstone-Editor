#ifndef PITCHSTONE_VULKAN_TEXTURE
#define PITCHSTONE_VULKAN_TEXTURE

#include <vulkan/vulkan.h>

class Texture {
public:
	Texture(VkDevice device, int width, int height);
	virtual ~Texture();
private:
	VkImage image;
	VkImageView view;
	VkSampler sampler;
	VkDeviceMemory memory;
};

#endif