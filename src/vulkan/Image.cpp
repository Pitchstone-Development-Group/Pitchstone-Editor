#include "Image.hpp"
#include <iostream>

Image::Image(Device *device, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage) {
	m_device = device;
	m_format = format;
	m_width = width, m_height = height;

	VkImageCreateInfo info_img{};
	info_img.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	info_img.pNext = VK_NULL_HANDLE;
	info_img.flags = 0;
	info_img.format = format;
	info_img.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	info_img.tiling = VK_IMAGE_TILING_OPTIMAL;
	info_img.imageType = VK_IMAGE_TYPE_2D;
	info_img.mipLevels = 1;
	info_img.arrayLayers = 1;
	info_img.samples = VK_SAMPLE_COUNT_1_BIT;
	info_img.extent = {width, height, 1};
	info_img.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	info_img.usage = usage;

	m_device->create(&info_img, &m_image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false);

	VkImageViewCreateInfo info_viw{};
	info_viw.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	info_viw.pNext = VK_NULL_HANDLE;
	info_viw.flags = 0;
	info_viw.image = m_image;
	info_viw.format = format;
	info_viw.viewType = VK_IMAGE_VIEW_TYPE_2D;
	info_viw.components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
	info_viw.subresourceRange.layerCount = 1;
	info_viw.subresourceRange.baseArrayLayer = 0;
	info_viw.subresourceRange.levelCount = 1;
	info_viw.subresourceRange.baseMipLevel = 0;
	info_viw.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

	vkCreateImageView(m_device->device(), &info_viw, VK_NULL_HANDLE, &m_view);
}

Image::~Image() {
	vkDestroyImageView(m_device->device(), m_view, VK_NULL_HANDLE);
	m_device->destroy(m_image);
}

void Image::barrier(VkCommandBuffer buffer, VkImage image, VkAccessFlags2 srcAccess, VkAccessFlags2 dstAccess, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags2 source, VkPipelineStageFlags2 destination) {
	VkImageMemoryBarrier2 barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	barrier.image = image;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.srcAccessMask = srcAccess;
	barrier.dstAccessMask = dstAccess;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.srcStageMask = source;
	barrier.dstStageMask = destination;

	VkDependencyInfo dependency{};
	dependency.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	dependency.dependencyFlags = 0;
	dependency.imageMemoryBarrierCount = 1;
	dependency.pImageMemoryBarriers = &barrier;
	vkCmdPipelineBarrier2(buffer, &dependency);	
}

void Image::barrier(VkCommandBuffer buffer, VkAccessFlags2 srcAccess, VkAccessFlags2 dstAccess, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags2 source, VkPipelineStageFlags2 destination) {
	VkImageMemoryBarrier2 barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	barrier.image = m_image;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.srcAccessMask = srcAccess;
	barrier.dstAccessMask = dstAccess;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.srcStageMask = source;
	barrier.dstStageMask = destination;

	VkDependencyInfo dependency{};
	dependency.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	dependency.dependencyFlags = 0;
	dependency.imageMemoryBarrierCount = 1;
	dependency.pImageMemoryBarriers = &barrier;
	vkCmdPipelineBarrier2(buffer, &dependency);	
}