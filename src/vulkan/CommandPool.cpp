#include "CommandPool.hpp"

CommandPool::CommandPool(Device *device, uint32_t buffers, uint32_t index) {
	m_device = device;

	VkCommandPoolCreateInfo pool{};
	pool.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool.queueFamilyIndex = index;

	vkCreateCommandPool(m_device->device(), &pool, VK_NULL_HANDLE, &m_pool);

	VkCommandBufferAllocateInfo allocate{};
	allocate.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocate.commandPool = m_pool;
	allocate.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocate.commandBufferCount = 1;
	m_buffers.resize(buffers);
	vkAllocateCommandBuffers(m_device->device(), &allocate, m_buffers.data());
}

CommandPool::~CommandPool() {
	vkFreeCommandBuffers(m_device->device(), m_pool, (uint32_t)m_buffers.size(), m_buffers.data());
	vkDestroyCommandPool(m_device->device(), m_pool, VK_NULL_HANDLE);
}