#pragma once

#include "Device.hpp"

class Buffer {
public:
	Buffer(Device *device, VkBufferUsageFlags usage, VkDeviceSize size, std::vector<VkMemoryPropertyFlags>& choices, bool alone);
	~Buffer();

	VkBuffer buffer() { return m_buffer; }
	VkDeviceSize size() { return m_size; }

	void *map();
	void unmap();
private:
	VkBuffer m_buffer;
	Device *m_device;
	VkDeviceSize m_size;
};