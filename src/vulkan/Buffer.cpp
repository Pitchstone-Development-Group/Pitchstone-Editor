#include "Buffer.hpp"
#include <stdlib.h>
#include <string.h>

Buffer::Buffer(Device *device, VkBufferUsageFlags usage, VkDeviceSize size, std::vector<VkMemoryPropertyFlags>& choices, bool alone) {
	m_device = device;
	VkBufferCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	info.size = size;
	info.usage = usage;

	VkResult result = VK_ERROR_UNKNOWN;
	for (auto choice : choices) {
		result = device->create(&info, &m_buffer, choice, alone);
		if (result == VK_SUCCESS)
			break;
	}
	if (result!= VK_SUCCESS)
		throw std::runtime_error("Failed to allocate buffer!");
}

Buffer::~Buffer() {
	m_device->destroy(m_buffer);
}

void *Buffer::map() {
	return m_device->map(m_buffer);
}

void Buffer::unmap() {
	m_device->unmap(m_buffer);
}