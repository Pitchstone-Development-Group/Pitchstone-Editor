#ifndef PITCHSTONE_VULKAN_QUEUEMANAGER
#define PITCHSTONE_VULKAN_QUEUEMANAGER

#include <vulkan/vulkan.h>
#include <vector>
#include <map>
#include <mutex>
#include "Queue.hpp"

class QueueManager {
public:
	QueueManager(VkPhysicalDevice physical, VkSurfaceKHR surface);
	virtual ~QueueManager();
	Queue* getQueue(int i) { return m_queues[m_qindex[i]]; }
	std::vector<uint32_t> m_count;
private:
	uint32_t m_qindex[4];
	std::vector<Queue*> m_queues;
};

#endif
