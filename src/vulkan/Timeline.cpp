#include "Timeline.hpp"

Timeline::Timeline(Device *device) {
	m_device = device;

	VkSemaphoreTypeCreateInfo type{};
	type.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
	type.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;

	VkSemaphoreCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	info.pNext = &type;
	vkCreateSemaphore(m_device->device(), &info, VK_NULL_HANDLE, &m_semaphore);
}

Timeline::~Timeline() {
	vkDestroySemaphore(m_device->device(), m_semaphore, VK_NULL_HANDLE);
}

uint64_t Timeline::get() {
	uint64_t value;
	vkGetSemaphoreCounterValue(m_device->device(), m_semaphore, &value);
	return value;
}

VkResult Timeline::wait(uint64_t value, uint64_t timer) {
	VkSemaphoreWaitInfo info{};
	info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
	info.semaphoreCount = 1;
	info.pSemaphores = &m_semaphore;
	info.pValues = &value;

	return vkWaitSemaphores(m_device->device(), &info, timer);
}

void Timeline::signal(uint64_t value) {
	VkSemaphoreSignalInfo info{};
	info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
	info.semaphore = m_semaphore;
	info.value = value;

	vkSignalSemaphore(m_device->device(), &info);
}