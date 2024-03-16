#pragma once

#include "CommandPool.hpp"
#include "Device.hpp"

class FiFSyncs {
public:
	FiFSyncs(Device *device, uint32_t family);
	~FiFSyncs();

	uint32_t index() { return m_index; }
	void swap() { m_index ^= 1; }
	void wait() {vkWaitForFences(m_device->device(), 1, &m_fence[m_index], VK_TRUE, UINT64_MAX); }
	void reset() {vkResetFences(m_device->device(), 1, &m_fence[m_index]); }

	VkCommandBuffer buffer() {vkResetCommandPool(m_device->device(), m_pool[m_index]->pool(), 0); return m_pool[m_index]->buffer(0); }
	
	VkSemaphore acquire() { return m_acquire[m_index]; }
	VkSemaphore blit() { return m_blit[m_index]; }
	VkFence fence() { return m_fence[m_index]; }
private:
	Device *m_device;
	uint32_t m_index;
	VkSemaphore m_acquire[2];
	VkSemaphore m_blit[2];
	VkFence m_fence[2];
	CommandPool *m_pool[2];
};