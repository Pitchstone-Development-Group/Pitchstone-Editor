#ifndef PITCHSTONE_VULKAN_QUEUE
#define PITCHSTONE_VULKAN_QUEUE

#include <vulkan/vulkan.h>
#include <mutex>

class Queue {
public:
	Queue() { m_family = m_index = (uint32_t)-1; }
	virtual ~Queue() { m_family = m_index = (uint32_t)-1; m_queue = NULL; }
	Queue(uint32_t f, uint32_t i) { m_family = f, m_index = i; }
	Queue(const Queue& q) { m_family = q.m_family, m_index = q.m_index; }
	Queue operator=(const Queue& q) { m_family = q.m_family, m_index = q.m_index; return *this; };
	VkQueue m_queue = NULL;
	uint32_t m_family = (uint32_t) -1, m_index = (uint32_t) -1;

	uint32_t threads() { return m_threads; }
	uint32_t owner() { return m_owner; }
	void addThread(uint32_t thread) { m_threads |= thread; }

	bool lock(uint32_t thread) {
		if (!(m_threads & thread))
			return false;
		if (m_owner == thread)
			return true;
		m_lock.lock();
		m_owner = thread;
		return true;
	}
	
	bool unlock(uint32_t thread) {
		if (!(m_threads & thread))
			return false;
		if (m_owner != thread)
			return false;
		m_lock.unlock();
		m_owner = 0;
		return true;
	}

private:
	std::mutex m_lock = std::mutex();
	uint32_t m_threads;
	uint32_t m_owner;
};

#endif
