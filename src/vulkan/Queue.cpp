#include "Queue.hpp"
#include <iostream>

struct QueuePreferences {
	VkQueueFlags must;
	VkQueueFlags mustNot;
};

static const VkQueueFlags GRAPHICS = VK_QUEUE_GRAPHICS_BIT;
static const VkQueueFlags COMPUTE  = VK_QUEUE_COMPUTE_BIT;
static const VkQueueFlags TRANSFER = VK_QUEUE_TRANSFER_BIT;
static const VkQueueFlags DECODE   = VK_QUEUE_VIDEO_DECODE_BIT_KHR;
static const VkQueueFlags ENCODE   = VK_QUEUE_VIDEO_ENCODE_BIT_KHR;

/* This selection assumes QUEUE_WINDOW_GRAPHICS could not support present
 * Because otherwise, QUEUE_WINDOW_PRESENT and QUEUE_WINDOW_GRAPHICS
 * would become a singular queue
 */

static std::vector<QueuePreferences> queuePreferencesEngineGraphics = {
	{ (GRAPHICS | COMPUTE), (DECODE | ENCODE) },
	{ (GRAPHICS | COMPUTE), (0) },
	{ (GRAPHICS | COMPUTE), (DECODE | ENCODE) },
	{ (GRAPHICS | COMPUTE), (0) },
	{ (GRAPHICS), (DECODE | ENCODE) },
	{ (GRAPHICS), (0) },
	{ (GRAPHICS), (DECODE | ENCODE) },
	{ (GRAPHICS), (0) }
};

/* This selection assumes QUEUE_ENGINE_GRAPHICS could not support compute
 * Because otherwise, QUEUE_ENGINE_GRAPHICS and QUEUE_ENGINE_COMPUTE
 * would become a singular queue
 */
static std::vector<QueuePreferences> queuePreferencesEngineCompute = {
	{ (COMPUTE) , (DECODE | ENCODE) },
	{ (COMPUTE), (0) },
	{ (COMPUTE) , (DECODE | ENCODE) },
	{ (COMPUTE), (0) }
};


static std::vector<QueuePreferences> queuePreferencesConvertorCompute = {
	{ (COMPUTE) , (GRAPHICS | DECODE | ENCODE) },
	{ (COMPUTE) , (GRAPHICS) },
	{ (COMPUTE) , (GRAPHICS | DECODE | ENCODE) },
	{ (COMPUTE) , (GRAPHICS) },
	{ (COMPUTE), (0) },
	{ (COMPUTE), (0) }
};

static std::vector<QueuePreferences> queuePreferencesConvertorTransfer = {
    { (TRANSFER), (GRAPHICS | COMPUTE | DECODE | ENCODE) },
    { (TRANSFER), (GRAPHICS | COMPUTE) },
    { (TRANSFER), (GRAPHICS | COMPUTE | DECODE | ENCODE) },
    { (TRANSFER), (GRAPHICS | COMPUTE) },
    { (TRANSFER), (COMPUTE) },
    { (TRANSFER), (GRAPHICS) },
    { (TRANSFER), (COMPUTE) },
    { (TRANSFER), (GRAPHICS) },
    { (COMPUTE) , (GRAPHICS | DECODE | ENCODE) },
    { (COMPUTE) , (GRAPHICS) },
    { (COMPUTE) , (GRAPHICS | DECODE | ENCODE) },
    { (COMPUTE) , (GRAPHICS) },
    { (COMPUTE), (0) },
    { (COMPUTE), (0) },
    { (GRAPHICS), (DECODE | ENCODE) },
    { (GRAPHICS), (0) },
    { (GRAPHICS), (DECODE | ENCODE) },
    { (GRAPHICS), (0) }
};

Queue::Queue(VkDevice dev, uint32_t fam, uint32_t ind, bool alone) {
	m_family = fam;
	m_index = ind;
	m_alone = alone;
	vkGetDeviceQueue(dev, fam, ind, &m_queue);
}

Queue::~Queue() {

}

static inline std::pair<uint32_t, uint32_t> findFamilyIndex(const std::vector<VkQueueFamilyProperties>& properties, const std::vector<QueuePreferences>& preferences, std::vector<uint32_t> usedFamilies = {}) {
	for (auto& pref : preferences) {
		for (uint32_t fam = 0; fam < (uint32_t) properties.size(); ++fam) {
			if (((properties[fam].queueFlags & pref.must) == pref.must) & ((properties[fam].queueFlags & pref.mustNot) == 0)) {
				uint32_t ind = 0;
				for (auto uf : usedFamilies) {
					if (uf == fam) {
						ind++;
					}
				}
				if (ind < properties[fam].queueCount) {
					return std::pair<uint32_t, uint32_t>(fam, ind);
				}
			}
		}
	}
	return std::pair<uint32_t, uint32_t>(~0U, ~0U);
}

std::vector<std::pair<uint32_t, uint32_t>> Queue::assign(VkPhysicalDevice physical) {
	std::vector<std::pair<uint32_t, uint32_t>> ret(QUEUE_THREADS);
	for (int i = 0; i < QUEUE_THREADS; ++i) {
		ret[i] = {~0U, ~0U};
	}
	
	uint32_t count;
	vkGetPhysicalDeviceQueueFamilyProperties(physical, &count, VK_NULL_HANDLE);
	std::vector<VkQueueFamilyProperties> families(count);
	vkGetPhysicalDeviceQueueFamilyProperties(physical, &count, families.data());

	std::vector<uint32_t> usedFamilies;

	ret[QUEUE_CONVERTOR_TRANSFER] = findFamilyIndex(families, queuePreferencesConvertorTransfer);
	usedFamilies.push_back(ret[QUEUE_CONVERTOR_TRANSFER].first);

	ret[QUEUE_CONVERTOR_COMPUTE] = findFamilyIndex(families, queuePreferencesConvertorCompute, usedFamilies);
	if (ret[QUEUE_CONVERTOR_COMPUTE].first == ~0U) {
		ret[QUEUE_CONVERTOR_COMPUTE] = findFamilyIndex(families, queuePreferencesConvertorCompute);
	} else {
		usedFamilies.push_back(ret[QUEUE_CONVERTOR_COMPUTE].first);
	}

	/* Because the engine graphics and compute work will be on the same thread,
	 * we should combine them to one queue if we can
	 */
	ret[QUEUE_ENGINE_GRAPHICS] = findFamilyIndex(families, queuePreferencesEngineGraphics, usedFamilies);
	if (ret[QUEUE_ENGINE_GRAPHICS].first == ~0U) {
		ret[QUEUE_ENGINE_GRAPHICS] = findFamilyIndex(families, queuePreferencesEngineGraphics);
	} else {
		usedFamilies.push_back(ret[QUEUE_ENGINE_GRAPHICS].first);
	}

	if ((families[ret[QUEUE_ENGINE_GRAPHICS].first].queueFlags & COMPUTE) == COMPUTE) {
		ret[QUEUE_ENGINE_COMPUTE] = ret[QUEUE_ENGINE_GRAPHICS];
	} else {
		ret[QUEUE_ENGINE_COMPUTE] = findFamilyIndex(families, queuePreferencesEngineCompute, usedFamilies);
		if (ret[QUEUE_ENGINE_COMPUTE].first == ~0U) {
			ret[QUEUE_ENGINE_COMPUTE] = findFamilyIndex(families, queuePreferencesEngineCompute);
		} else {
			usedFamilies.push_back(ret[QUEUE_ENGINE_COMPUTE].first);
		}
	}

	return ret;
}

std::map<uint32_t, uint32_t> Queue::allocation(const std::vector<std::pair<uint32_t, uint32_t>>& assignments) {
	std::map<uint32_t, uint32_t> families;

	for (int i = 0; i < QUEUE_THREADS; ++i) {
		
		bool duplicate = false;
		for (int j = i - 1; j >= 0; --j) {
			if (assignments[i].first == assignments[j].first && assignments[i].second == assignments[j].second) {
				duplicate = true;
				break;
			}
		}

		if (!duplicate) {
			if (!families.contains(assignments[i].first)) {
				families[assignments[i].first] = 1;
			} else {
				families[assignments[i].first]++;
			}
		}
	}

	return families;
}

void Queue::lock() {
	if (m_alone)
		return;
	m_mutex.lock();
}

bool Queue::tryLock() {
	if (m_alone)
		return true;
	return m_mutex.try_lock();
}

void Queue::unlock() {
	if (m_alone)
		return;
	m_mutex.unlock();
}