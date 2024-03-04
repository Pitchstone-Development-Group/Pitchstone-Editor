#include "Allocator.hpp"
#include <iostream>

Allocator::Allocator(VkPhysicalDevice physical, VkDevice device) {
	m_device = device;
	vkGetPhysicalDeviceMemoryProperties(physical, &m_memory);

	setBestMemoryTypeOptions(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	setBestMemoryTypeOptions(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	setBestMemoryTypeOptions(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT);
	setBestMemoryTypeOptions(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	setBestMemoryTypeOptions(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT);
}
Allocator::~Allocator() {
	for (auto& h : m_heaps)
		h->entries.clear();
	m_heaps.clear();
	m_allocations.clear();
	for (auto& bo : m_bestOptions)
		bo.second.clear();
	m_bestOptions.clear();
}

VkResult Allocator::allocate(VkDeviceSize size, VkDeviceMemory *memory, VkMemoryPropertyFlags flags) {
	m_mutex.lock();
	VkResult result = VK_ERROR_FEATURE_NOT_PRESENT;
	if (!m_bestOptions.contains(flags))
		setBestMemoryTypeOptions(flags);
	if (m_bestOptions[flags].size() == 0) {
		m_mutex.unlock();
		return result;
	}
	for (auto type : m_bestOptions[flags]) {
		if (size > m_memory.memoryHeaps[m_memory.memoryTypes[type].heapIndex].size)
			continue;
		AllocatorHeap *ah = new AllocatorHeap;
		ah->entries = std::vector<AllocatorEntry*>();
		ah->type = type;
		ah->alone = true;
		VkMemoryAllocateInfo info_alloc{};
		info_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		info_alloc.memoryTypeIndex = type;
		info_alloc.allocationSize = (size + 4095U) & ~(4095U);
		result = vkAllocateMemory(m_device, &info_alloc, nullptr, &ah->memory);
		if (result != VK_SUCCESS) {
			delete ah;
			continue;
		}
		m_mutex.unlock();
		return VK_SUCCESS;
	}
	m_mutex.unlock();
	return result;
}

VkResult Allocator::allocate(VkBuffer buffer, VkMemoryPropertyFlags flags,  bool alone = false) {
	VkMemoryRequirements reqs;
	vkGetBufferMemoryRequirements(m_device, buffer, &reqs);
	AllocatorEntry *entry;
	AllocatorHeap *heap;
	VkResult result = allocate(reqs, flags, alone, &heap, &entry);
	if (result != VK_SUCCESS)
		return result;
	entry->buffer = buffer;
	entry->image = nullptr;
	if (!alone)
		heap->maplock.lock();
	result = vkBindBufferMemory(m_device, buffer, heap->memory, entry->offset);
	if (!alone)
		heap->maplock.unlock();
	return result;
}

VkResult Allocator::allocate(VkImage image, VkMemoryPropertyFlags flags,  bool alone = false) {
	VkMemoryRequirements reqs;
	vkGetImageMemoryRequirements(m_device, image, &reqs);
	AllocatorEntry *entry;
	AllocatorHeap *heap;
	VkResult result = allocate(reqs, flags, alone, &heap, &entry);
	if (result != VK_SUCCESS)
		return result;
	entry->buffer = nullptr;
	entry->image = image;
	if (!alone)
		heap->maplock.lock();
	result = vkBindImageMemory(m_device, image, heap->memory, entry->offset);
	if (!alone)
		heap->maplock.unlock();
	return result;
}

uint32_t Allocator::getAllocationID(VkBuffer buffer) {
	for (auto ae : m_allocations)
		if (ae.second != nullptr && ae.second->buffer == buffer)
			return ae.first;
	return ~0;
}

uint32_t Allocator::getAllocationID(VkImage image) {
	for (auto ae : m_allocations)
		if (ae.second != nullptr && ae.second->image == image)
			return ae.first;
	return ~0;
}

void* Allocator::map(VkBuffer buffer) {
	void *data;

	size_t hindex, eindex;
	m_mutex.lock();
	if (!findHeapAndEntry(buffer, nullptr, &hindex, &eindex))
		return nullptr;
	AllocatorHeap *heap = m_heaps[hindex];
	AllocatorEntry *entry = m_heaps[hindex]->entries[eindex];
	m_mutex.unlock();
	
	if (!heap->alone)
		heap->maplock.lock();
	VkResult result = vkMapMemory(m_device, heap->memory, entry->offset, entry->size, 0, &data);
	return (result == VK_SUCCESS) ? data : nullptr;
}

void Allocator::unmap(VkBuffer buffer) {
	size_t hindex, eindex;
	m_mutex.lock();
	if (!findHeapAndEntry(buffer, nullptr, &hindex, &eindex))
		return;
	AllocatorHeap *heap = m_heaps[hindex];
	AllocatorEntry *entry = m_heaps[hindex]->entries[eindex];
	m_mutex.unlock();

	if (m_heaps[hindex]->type & VK_MEMORY_PROPERTY_HOST_CACHED_BIT ) {
		VkMappedMemoryRange range{};
		range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		range.pNext = nullptr;
		range.memory = m_heaps[hindex]->memory;
		range.size = m_heaps[hindex]->entries[eindex]->size;
		range.offset = m_heaps[hindex]->entries[eindex]->offset;
		vkFlushMappedMemoryRanges(m_device, 1, &range);
	}

	vkUnmapMemory(m_device, m_heaps[hindex]->memory);
	m_heaps[hindex]->maplock.unlock();
}

void Allocator::free(uint32_t allocationID) {
	m_mutex.lock();
	if (!m_allocations.contains(allocationID) || m_allocations[allocationID] == nullptr) {
		m_mutex.unlock();
		return;
	}

	AllocatorEntry* ae = m_allocations[allocationID];
	size_t hindex, eindex;
	if (!findHeapAndEntry(ae->buffer, ae->image, &hindex, &eindex)) {
		m_mutex.unlock();
		return;
	}

	delete m_heaps[hindex]->entries[eindex];
	m_heaps[hindex]->entries.erase(m_heaps[hindex]->entries.begin() + eindex);
	if (m_heaps[hindex]->entries.empty()) {
		vkFreeMemory(m_device, m_heaps[hindex]->memory, nullptr);
		delete m_heaps[hindex];
		m_heaps.erase(m_heaps.begin() + hindex);
	}
	m_allocations[allocationID] = nullptr;
	m_mutex.unlock();
}

static VkDeviceSize getBlockSize(VkMemoryRequirements& reqs, AllocatorSizingMode mode) {
	static const VkDeviceSize maxBlockSizeMultiple = (1 << 27);
	VkDeviceSize multiple = 1 << (31 - __builtin_clz(reqs.size));
	switch (mode) {
		case ALLOCATOR_COMPRESSION_MAXBLOCK:
			return (reqs.size + maxBlockSizeMultiple - 1) & ~(maxBlockSizeMultiple - 1);
		case ALLOCATOR_COMPRESSION_MIN_POWER_OF2:
			if (multiple < reqs.alignment)
				multiple = reqs.alignment;
			if (multiple > maxBlockSizeMultiple)
				multiple = maxBlockSizeMultiple;
			return (reqs.size + multiple - 1) & ~(multiple - 1);
		default: // ALLCOATOR_COMPRESSION_EXACT_ALIGN
			return (reqs.size + reqs.alignment - 1) & ~(reqs.alignment - 1);
	}
}

bool Allocator::findHeapAndEntry(VkBuffer buffer, VkImage image, size_t *hindex, size_t *eindex) {
	for (size_t h = 0; h < m_heaps.size(); ++h) {
		for (size_t e = 0; e < m_heaps[h]->entries.size(); ++e) {
			if (m_heaps[h]->entries[e]->buffer == buffer && m_heaps[h]->entries[e]->image ==image) {
				*hindex = h;
				*eindex = e;
				return true;
			}
		}
	}
	return false;
}

uint32_t Allocator::getFirstAvailableAllocationID() {
	for (uint32_t id = 0; id < (uint32_t)(~0); ++id)
		if (!m_allocations.contains(id) || m_allocations[id] == nullptr)
			return id;
	return ~0;
}

static AllocatorEntry* findFirstAvailable(AllocatorHeap *heap, VkMemoryRequirements& reqs) {
	AllocatorEntry *ae = new AllocatorEntry;
	auto &entries = heap->entries;

	// First, check if there are holes for us to slide in, like your crush in others' DMs
	if (entries.size() > 1) {
		for (size_t e = 0; e < entries.size() - 1; e++) {
			VkDeviceSize offset = (entries[e]->offset + entries[e]->size + reqs.alignment - 1) & ~(reqs.alignment - 1);
			if ((offset + reqs.size) <=entries[e + 1]->offset) {
				ae->offset = offset;
				ae->size = reqs.size;
				entries.insert(entries.begin() + (e + 1), ae);
				return ae;
			}
		}
	}

	// Otherwise, see if it can be appended
	VkDeviceSize offset = (entries.empty()) ? 0 : ((entries[entries.size() - 1]->offset + entries[entries.size() - 1]->size + reqs.alignment - 1) & ~(reqs.alignment - 1));
	if ((offset + reqs.size) <= heap->size) {
		ae->offset = offset;
		ae->size = reqs.size;
		entries.push_back(ae);
		return ae;
	}

	delete ae;
	return nullptr;
}

VkResult Allocator::allocate(VkMemoryRequirements& reqs, VkMemoryPropertyFlags flags, bool alone, AllocatorHeap **heap, AllocatorEntry **entry) {
	m_mutex.lock();
	VkResult result = VK_ERROR_FEATURE_NOT_PRESENT;
	if (!m_bestOptions.contains(flags))
		setBestMemoryTypeOptions(flags);
	if (m_bestOptions[flags].size() == 0) {
		m_mutex.unlock();
		return result;
	}
	for (auto type : m_bestOptions[flags]) {
		if (reqs.size > m_memory.memoryHeaps[m_memory.memoryTypes[type].heapIndex].size)
			continue;
		if (!alone) {
			for (auto& h : m_heaps) {
				if (h->type == type && reqs.size < h->size && h->alone == alone) {
					AllocatorEntry *ae = findFirstAvailable(h, reqs);
					if (ae != nullptr) {
						*entry = ae;
						*heap = h;
						m_allocations[getFirstAvailableAllocationID()] = ae;
						m_mutex.unlock();
						return VK_SUCCESS;
					}
				}
			}
		}
		AllocatorHeap *ah = new AllocatorHeap;
		ah->entries = std::vector<AllocatorEntry*>();
		ah->type = type;
		ah->alone = alone;
		VkMemoryAllocateInfo info_alloc{};
		info_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		info_alloc.memoryTypeIndex = type;
		if (alone) {
			info_alloc.allocationSize = getBlockSize(reqs, ALLOCATOR_COMPRESSION_EXACT_ALIGN);
			result = vkAllocateMemory(m_device, &info_alloc, nullptr, &ah->memory);
			if (result != VK_SUCCESS) {
				delete ah;
				continue;
			}
		} else {
			info_alloc.allocationSize = getBlockSize(reqs, ALLOCATOR_COMPRESSION_MAXBLOCK);
			result = vkAllocateMemory(m_device, &info_alloc, nullptr, &ah->memory);
			if (result != VK_SUCCESS) {
				info_alloc.allocationSize = getBlockSize(reqs, ALLOCATOR_COMPRESSION_MIN_POWER_OF2);
				result = vkAllocateMemory(m_device, &info_alloc, nullptr, &ah->memory);
			}
			if (result != VK_SUCCESS) {
				info_alloc.allocationSize = getBlockSize(reqs, ALLOCATOR_COMPRESSION_EXACT_ALIGN);
				result = vkAllocateMemory(m_device, &info_alloc, nullptr, &ah->memory);
			}
			if (result != VK_SUCCESS) {
				delete ah;
				continue;
			}
		}
		ah->size = info_alloc.allocationSize;
		AllocatorEntry *ae = findFirstAvailable(ah, reqs);
		m_heaps.push_back(ah);
		*entry = ae;
		*heap = ah;
		m_allocations[getFirstAvailableAllocationID()] = ae;
		m_mutex.unlock();
		return VK_SUCCESS;
	}
	m_mutex.unlock();
	return result;
}

void Allocator::setBestMemoryTypeOptions(VkMemoryPropertyFlags flags) {
	m_bestOptions[flags] = std::vector<uint32_t>();
	
	std::vector<uint32_t> consistencies(m_memory.memoryTypeCount, 0);
	std::vector<uint32_t> discrepencies(m_memory.memoryTypeCount, 0);

	for (uint32_t t = 0; t < m_memory.memoryTypeCount; ++t) {
		consistencies[t] = flags & m_memory.memoryTypes[t].propertyFlags;
		discrepencies[t] = __builtin_popcount(flags ^ m_memory.memoryTypes[t].propertyFlags);
	}

	bool found;
	uint32_t minSift = 0, minDiscrepency;
	do {
		found = false;
		minDiscrepency = ~0;
		for (uint32_t t = 0; t < m_memory.memoryTypeCount; ++t) {
			if (consistencies[t] == flags && discrepencies[t] >= minSift && discrepencies[t] < minDiscrepency) {
				found = true;
				minDiscrepency = discrepencies[t];
			}
		}
		for (uint32_t t = 0; t < m_memory.memoryTypeCount; ++t) {
			if (consistencies[t] == flags && discrepencies[t] == minDiscrepency)
				m_bestOptions[flags].push_back(t);
		}

		minSift = minDiscrepency + 1;
	} while (found);
}