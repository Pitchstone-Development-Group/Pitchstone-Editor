#include "Image.hpp"

Image::Image(Device *device, uint32_t width, uint32_t height, VkFormat format, VkMemoryPropertyFlags flags) {
	m_device = device;
	m_format = format;

	VkImageCreateInfo info_img{};
	info_img.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	info_img.pNext = VK_NULL_HANDLE;
	info_img.flags = 0;
	info_img.format = format;
	info_img.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	info_img.imageType = VK_IMAGE_TYPE_2D;
	info_img.mipLevels = 1;
	info_img.arrayLayers = 1;
	info_img.samples = VK_SAMPLE_COUNT_1_BIT;
	info_img.extent = {width, height, 1};
	info_img.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	info_img.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
	info_img.queueFamilyIndexCount = 0;
	info_img.pQueueFamilyIndices = VK_NULL_HANDLE;
	
	m_device->create(&info_img, &m_image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false);

	VkImageViewCreateInfo info_viw{};
	info_viw.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	info_viw.pNext = VK_NULL_HANDLE;
	info_viw.flags = 0;
	info_viw.image = m_image;
	info_viw.viewType = VK_IMAGE_VIEW_TYPE_2D;
	info_viw.components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
	info_viw.subresourceRange.layerCount = 1;
	info_viw.subresourceRange.baseArrayLayer = 0;
	info_viw.subresourceRange.levelCount = 1;
	info_viw.subresourceRange.baseMipLevel = 0;
	info_viw.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

	vkCreateImageView(m_device->device(), &info_viw, VK_NULL_HANDLE, &m_view);
}