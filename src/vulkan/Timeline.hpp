#pragma once

#include "Device.hpp"

class Timeline {
public:
	Timeline(Device *device);
	~Timeline();

	VkSemaphore semaphore() { return m_semaphore; }

	uint64_t get();
	VkResult wait(uint64_t value, uint64_t timer);
	void signal(uint64_t value);

private:
	Device *m_device;
	VkSemaphore m_semaphore;
};