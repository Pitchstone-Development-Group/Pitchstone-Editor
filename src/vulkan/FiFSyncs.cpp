#include "FiFSyncs.hpp"

FiFSyncs::FiFSyncs(Device *device, uint32_t family) {
	m_device = device;
	m_index = 0;

	VkSemaphoreCreateInfo semaphore{};
	semaphore.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	vkCreateSemaphore(m_device->device(), &semaphore, VK_NULL_HANDLE, &m_acquire[0]);
	vkCreateSemaphore(m_device->device(), &semaphore, VK_NULL_HANDLE, &m_acquire[1]);
	vkCreateSemaphore(m_device->device(), &semaphore, VK_NULL_HANDLE, &m_blit[0]);
	vkCreateSemaphore(m_device->device(), &semaphore, VK_NULL_HANDLE, &m_blit[1]);

	VkFenceCreateInfo fence{};
	fence.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	vkCreateFence(m_device->device(), &fence, VK_NULL_HANDLE, &m_fence[0]);
	vkCreateFence(m_device->device(), &fence, VK_NULL_HANDLE, &m_fence[1]);

	m_pool[0] = new CommandPool(m_device, 1, family);
	m_pool[1] = new CommandPool(m_device, 1, family);
}

FiFSyncs::~FiFSyncs() {
	vkDestroyFence(m_device->device(), m_fence[0], VK_NULL_HANDLE);
	vkDestroyFence(m_device->device(), m_fence[1], VK_NULL_HANDLE);
	vkDestroySemaphore(m_device->device(), m_acquire[0], VK_NULL_HANDLE);
	vkDestroySemaphore(m_device->device(), m_acquire[1], VK_NULL_HANDLE);
	vkDestroySemaphore(m_device->device(), m_blit[0], VK_NULL_HANDLE);
	vkDestroySemaphore(m_device->device(), m_blit[1], VK_NULL_HANDLE);

	delete m_pool[0];
	delete m_pool[1];
}