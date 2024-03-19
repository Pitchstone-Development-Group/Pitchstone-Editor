#pragma once

#include "Device.hpp"

class CommandPool {
public:
	CommandPool(Device *device, uint32_t buffers, uint32_t index);
	~CommandPool();

	void reset() { vkResetCommandPool(m_device->device(), m_pool, 0); }

	VkCommandPool pool() { return m_pool; }
	VkCommandBuffer buffer(uint32_t index) { return m_buffers[index]; }

private:
	Device *m_device;
	VkCommandPool m_pool;
	std::vector<VkCommandBuffer> m_buffers;
};
