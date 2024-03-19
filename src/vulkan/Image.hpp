#pragma once

#include "Device.hpp"

class Image {
public:
	Image(Device *device, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage);
	~Image();

	VkImage image() { return m_image; }
	VkImageView view() { return m_view; }
	VkFormat format() { return m_format; }

	uint32_t width() { return m_width; }
	uint32_t height() { return m_height; }

	static void barrier(VkCommandBuffer buffer, VkImage, VkAccessFlags2 srcAccess, VkAccessFlags2 dstAccess, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags2 source, VkPipelineStageFlags2 destination);
	void barrier(VkCommandBuffer buffer, VkAccessFlags2 srcAccess, VkAccessFlags2 dstAccess, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags2 source, VkPipelineStageFlags2 destination);

private:
	Device *m_device;
	VkImage m_image;
	VkFormat m_format;
	VkImageView m_view;
	uint32_t m_width, m_height;
};