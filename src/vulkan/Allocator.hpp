#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <map>
#include <mutex>

enum AllocatorSizingMode {
	ALLOCATOR_COMPRESSION_MAXBLOCK,
	ALLOCATOR_COMPRESSION_MIN_POWER_OF2,
	ALLOCATOR_COMPRESSION_EXACT_ALIGN
};

struct AllocatorEntry { 
	VkBuffer buffer;
	VkImage image;
	VkDeviceSize size;
	VkDeviceSize offset;
};

struct AllocatorHeap {
	VkDeviceMemory memory;
	uint32_t type;
	VkDeviceSize size;
	std::vector<AllocatorEntry*> entries;
	std::mutex maplock;
	bool alone;
};

class Allocator {
public:
	Allocator(VkPhysicalDevice physical, VkDevice device, PFN_vkSetDeviceMemoryPriorityEXT priority);
	~Allocator();

	VkResult allocate(VkDeviceSize size, VkDeviceMemory *memory, VkMemoryPropertyFlags flags);
	VkResult allocate(VkBuffer buffer, VkMemoryPropertyFlags flags, bool alone);
	VkResult allocate(VkImage image, VkMemoryPropertyFlags flags, bool alone);

	uint32_t getAllocationID(VkBuffer buffer);
	uint32_t getAllocationID(VkImage image);

	void* map(VkBuffer buffer);
	void unmap(VkBuffer buffer);

	void free(VkDeviceMemory memory);
	void free(uint32_t allocationID);
private:
	bool findHeapAndEntry(VkBuffer buffer, VkImage image, size_t *hindex, size_t *eindex);
	uint32_t getFirstAvailableAllocationID();
	VkResult allocate(VkMemoryRequirements& reqs, VkMemoryPropertyFlags flags, bool alone, AllocatorHeap **heap, AllocatorEntry **entry);
	void setBestMemoryTypeOptions(VkMemoryPropertyFlags flags);

	PFN_vkSetDeviceMemoryPriorityEXT setPriority;
	
	VkPhysicalDeviceMemoryProperties m_memory;
	VkDevice m_device;
	std::map<uint32_t, AllocatorEntry*> m_allocations;
	std::vector<AllocatorHeap*> m_heaps;
	std::map<VkMemoryPropertyFlags, std::vector<uint32_t>> m_bestOptions;
	std::mutex m_mutex;
};
