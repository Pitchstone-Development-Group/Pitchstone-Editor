#pragma once

#include "Instance.hpp"
#include <mutex>
#include <map>

enum QueueThread : int {
	QUEUE_CONVERTOR_COMPUTE,
	QUEUE_CONVERTOR_TRANSFER,
	QUEUE_ENGINE_GRAPHICS,
	QUEUE_ENGINE_COMPUTE,
	QUEUE_THREADS
};

class Queue {
public:
	Queue(VkDevice dev, uint32_t fam, uint32_t ind, bool alone);
	~Queue();

	static std::vector<std::pair<uint32_t, uint32_t>> assign(VkPhysicalDevice physical);
	static std::map<uint32_t, uint32_t> allocation(const std::vector<std::pair<uint32_t, uint32_t>>& assignments);

	void lock();
	bool tryLock();

	inline VkQueue queue() { return m_queue; }
	inline uint32_t family() { return m_family; }
	inline uint32_t index() { return m_index; }

	void unlock();
private:
	VkQueue m_queue;
	uint32_t m_family, m_index;
	bool m_alone;
	std::mutex m_mutex;
};