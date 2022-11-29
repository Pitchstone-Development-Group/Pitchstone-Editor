#ifndef PITCHSTONE_VULKAN_QUEUEMANAGER
#define PITCHSTONE_VULKAN_QUEUEMANAGER

#include <vulkan/vulkan.h>
#include <vulkan.hpp>
#include <mutex>

class QueueManager {
public:
    QueueManager(VkInstance instance, int queues);
    virtual ~QueueManager();

    size_t queueSize() { return queues.size(); }
    void lock() { mutex.lock(); }
    void unlock() { mutex.unlock(); }

    VkQueueFamilyProperties getFamily(int i);
    VkQueue getQueue(int index);
    VkQueue getQueue(int family, int index);
private:
    std::mutex mutex;
    std::vector<VkQueueFamilyProperties> props;
    std::vector<VkQueue> queues;
};

#endif
