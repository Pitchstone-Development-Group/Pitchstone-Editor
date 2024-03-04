#pragma once

#include "Device.hpp"

class Image {
public:
	Image(Device *device, uint32_t width, uint32_t height, VkFormat format, VkMemoryPropertyFlags flags);
	~Image();

	VkImage image() { return m_image; }
	VkImageView view() { return m_view; }
	VkFormat format() { return m_format; }

private:
	Device *m_device;
	VkImage m_image;
	VkFormat m_format;
	VkImageView m_view;
};