#ifndef PITCHSTONE_VULKAN_TEXTURE
#define PITCHSTONE_VULKAN_TEXTURE

#include <vulkan/vulkan.h>

class Texture {
	Texture(VkDevice device, VkCommandPool pool, int width, int height);
}

#endif