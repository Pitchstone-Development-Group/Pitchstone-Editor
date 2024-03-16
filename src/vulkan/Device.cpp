#include "Device.hpp"
#include "Queue.hpp"
#include <iostream>
#include <bit>
#include <cstring>

DeviceProperties::DeviceProperties() {
	properties_10 = {};
	properties_10.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
	properties_10.pNext = &properties_11;
	properties_11 = {};
	properties_11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;
	properties_11.pNext = &properties_12;
	properties_12 = {};
	properties_12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;
	properties_12.pNext = &properties_13;
	properties_13 = {};
	properties_13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES;

	features_10 = {};
	features_10.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	features_10.pNext = &features_11;
	features_11 = {};
	features_11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
	features_11.pNext = &features_12;
	features_12 = {};
	features_12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	features_12.pNext = &features_13;
	features_13 = {};
	features_13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
}

DeviceProperties::DeviceProperties(VkPhysicalDevice physical, bool blank) : DeviceProperties() {
	vkGetPhysicalDeviceProperties2(physical, &properties_10);

	if (blank) {
		vkGetPhysicalDeviceFeatures2(physical, &features_10);
	}

	vkGetPhysicalDeviceMemoryProperties(physical, &memory);
}

DeviceProperties::~DeviceProperties() {

}

static uint32_t selectBestPhysicalDevice(std::vector<VkPhysicalDevice> physicals, VkSurfaceKHR surface) {
	VkPhysicalDeviceType preferredTypes[5] = { VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU, VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU, VK_PHYSICAL_DEVICE_TYPE_CPU, VK_PHYSICAL_DEVICE_TYPE_OTHER};
	uint32_t bestType = 4;

	uint32_t bestIndex = (uint32_t)physicals.size();

	VkDeviceSize bestMemory = 0;

	for (uint32_t p = 0; p < (uint32_t)physicals.size(); ++p) {
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(physicals[p], &properties);

		if (properties.apiVersion < VK_API_VERSION_1_3) {
			continue;
		}

		VkPhysicalDeviceMemoryProperties memories;
		vkGetPhysicalDeviceMemoryProperties(physicals[p], &memories);
		bool found2GB = false;
		for (uint32_t h = 0; h < memories.memoryHeapCount; ++h) {
			if (memories.memoryHeaps[h].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT &&  memories.memoryHeaps[h].size >= (1ULL << 31) ) {
				found2GB = true;
				break;
			}
		}
		if (!found2GB) {
			continue;
		}

		bool foundGraphics = false, foundCompute = false, foundPresent = false;
		uint32_t numQueues;
		vkGetPhysicalDeviceQueueFamilyProperties(physicals[p], &numQueues, VK_NULL_HANDLE);
		std::vector<VkQueueFamilyProperties> queues(numQueues);
		vkGetPhysicalDeviceQueueFamilyProperties(physicals[p], &numQueues, queues.data());
		for (uint32_t q = 0; q < numQueues; ++q) {
			if (queues[q].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				foundGraphics = true;
			}
			if (queues[q].queueFlags & VK_QUEUE_COMPUTE_BIT) {
				foundCompute = true;
			}
			VkBool32 present;
			(void)vkGetPhysicalDeviceSurfaceSupportKHR(physicals[p], q, surface, &present);
			if (present == VK_TRUE) {
				foundPresent = true;
			}
		}
		if (!foundGraphics || !foundCompute || !foundPresent) {
			continue;
		}

		uint32_t type;
		for (type = 0; type < 5; type++) {
			if (properties.deviceType == preferredTypes[type]) {
				break;
			}
		}
		if (type < bestType) {
			bestIndex = p;
			bestType = type;
		} else if (type > bestType) {
			continue;
		} else {
			VkDeviceSize size = 0;
			for (uint32_t h = 0; h < memories.memoryHeapCount; ++h) {
				if (memories.memoryHeaps[h].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
					size = std::max(size, memories.memoryHeaps[h].size);
				}
			}
			if (size > bestMemory) {
				bestMemory = size;
				bestIndex = p;
			}
		}

		VkPhysicalDeviceFeatures features;
		vkGetPhysicalDeviceFeatures(physicals[p], &features);
	}

	return bestIndex;
}

Device::Device(Instance *instance, VkSurfaceKHR surface) {
	(void) surface;

	m_device = VK_NULL_HANDLE;
	m_instance = instance->instance();
	uint32_t count;
	vkEnumeratePhysicalDevices(m_instance, &count, VK_NULL_HANDLE);
	m_physicals.resize(count);
	vkEnumeratePhysicalDevices(m_instance, &count, m_physicals.data());

	m_physical = selectBestPhysicalDevice(m_physicals, surface);
	
	auto queueAssignments = Queue::assign(m_physicals[m_physical], surface);

	auto queueFamilies = Queue::allocation(queueAssignments);

	std::vector<float> priorities(QUEUE_THREADS);
	for (float& p : priorities) {
		p = 1.0F;
	}

	std::vector<VkDeviceQueueCreateInfo> info_que(queueFamilies.size());
	uint32_t type = 0;
	for (auto& qf : queueFamilies) {
		info_que[type].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		info_que[type].pNext = VK_NULL_HANDLE;
		info_que[type].flags = 0;
		info_que[type].queueFamilyIndex = qf.first;
		info_que[type].queueCount = qf.second;
		info_que[type].pQueuePriorities = priorities.data();
		type++;
	}
	

	std::vector<const char*> extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	m_deviceMemoryPriority = false;
	vkEnumerateDeviceExtensionProperties(m_physicals[m_physical], VK_NULL_HANDLE, &count, VK_NULL_HANDLE);
	std::vector<VkExtensionProperties> available(count);
	vkEnumerateDeviceExtensionProperties(m_physicals[m_physical], VK_NULL_HANDLE, &count, available.data());
	for (auto a : available) {
		if (strncmp(a.extensionName, VK_EXT_PAGEABLE_DEVICE_LOCAL_MEMORY_EXTENSION_NAME, 256) == 0) {
			extensions.push_back(VK_EXT_PAGEABLE_DEVICE_LOCAL_MEMORY_EXTENSION_NAME);
			extensions.push_back(VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME);
			m_deviceMemoryPriority = true;
			break;
		}
	}

	m_properties.features_12.timelineSemaphore = VK_TRUE;
	m_properties.features_13.dynamicRendering = VK_TRUE;
	m_properties.features_13.synchronization2 = VK_TRUE;

	VkDeviceCreateInfo info_dev{};
	info_dev.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	info_dev.pNext = &m_properties.features_11;

	info_dev.flags = 0;
	info_dev.queueCreateInfoCount = (uint32_t)info_que.size();
	info_dev.pQueueCreateInfos = info_que.data();
	info_dev.enabledLayerCount = 0;
	info_dev.ppEnabledLayerNames = VK_NULL_HANDLE;
	info_dev.enabledExtensionCount = (uint32_t)extensions.size();
	info_dev.ppEnabledExtensionNames = extensions.data();
	info_dev.pEnabledFeatures = &m_properties.features_10.features;

	vkCreateDevice(m_physicals[m_physical], &info_dev, VK_NULL_HANDLE, &m_device);

	for (int i = 0; i < QUEUE_THREADS; ++i) {
		m_queuesIndex[i] = QUEUE_THREADS;
	}
	int unique = 0;
	for (int i = 0; i < QUEUE_THREADS; ++i) {
		if (m_queuesIndex[i] != QUEUE_THREADS) {
			continue;
		}
		bool alone = true;
		for (int j = i + 1; j < QUEUE_THREADS; ++j) {
			if (queueAssignments[i].first == queueAssignments[j].first && queueAssignments[i].second == queueAssignments[j].second) {
				m_queuesIndex[j] = unique;

				/* QUEUE_WINDOW_GRAPHICS and QUEUE_WINDOW_PRESENT, even if one VkQueue, is shared between two threads */
				if (i == QUEUE_CONVERTOR_COMPUTE && j == QUEUE_CONVERTOR_TRANSFER) {
					continue;
				} else if (i == QUEUE_ENGINE_GRAPHICS && j == QUEUE_ENGINE_COMPUTE) {
					continue;
				}

				alone = false;
			}
		}
		m_queuesIndex[i] = unique;
		m_queues.push_back(new Queue(m_device, queueAssignments[i].first, queueAssignments[i].second, alone));
		unique++;
	}

	vkGetPhysicalDeviceProperties2(m_physicals[m_physical], &m_properties.properties_10);

	PFN_vkSetDeviceMemoryPriorityEXT priority = m_deviceMemoryPriority ? (PFN_vkSetDeviceMemoryPriorityEXT) vkGetInstanceProcAddr(m_instance, "vkSetDeviceMemoryPriorityEXT") : VK_NULL_HANDLE;
	m_allocator = new Allocator(m_physicals[m_physical], m_device, priority);
}

Device::~Device() {
	for (Queue* queue : m_queues) {
		delete queue;
	}
	delete m_allocator;
	vkDestroyDevice(m_device, VK_NULL_HANDLE);
}

void Device::localSizes(uint32_t width, uint32_t height, uint32_t *sizeX, uint32_t *sizeY) {
	uint32_t maxInv = m_properties.properties_10.properties.limits.maxComputeWorkGroupInvocations;
	uint32_t maxX = m_properties.properties_10.properties.limits.maxComputeWorkGroupSize[0];
	uint32_t maxY = m_properties.properties_10.properties.limits.maxComputeWorkGroupSize[1];

	uint32_t minCost = ~0U;
	uint32_t maxGroups = 0;

	for (uint32_t n = 0; n <= 6; ++n) {
		uint32_t startY = 1 << n, stepY = std::max(1 << (n + 1), 64), stepX = 64 >> n;
		for (uint32_t y = startY; y <= maxY; y += stepY) {
			uint32_t boundX = std::min(maxX, maxInv / y);
			for (uint32_t x = stepX; x <= boundX; x += stepX) {
				uint32_t groups = x * y;
				uint32_t ingroups  = (width / x) * (height / y);
				uint32_t outgroups = (((width - 1) / x) + 1) * (((height - 1) / y) + 1);
				uint32_t cost = ingroups + ((outgroups - ingroups) << 1);

				if (cost < minCost) {
					*sizeX = x;
					*sizeY = y;
					minCost = cost;
					maxGroups = groups;
					if (maxGroups == maxInv && outgroups == ingroups) {
						return;
					}
				} else if (cost == minCost) {
					if (groups > maxGroups) {
						*sizeX = x;
						*sizeY = y;
						maxGroups = groups;
						if (maxGroups == maxInv && outgroups == ingroups) {
							return;
						}
					}
				}
			}
		}
	}
}

VkResult Device::create(VkDeviceSize size, VkDeviceMemory *memory, VkMemoryPropertyFlags flags) {
	return m_allocator->allocate(size, memory, flags);
}

VkResult Device::create(VkBufferCreateInfo* info, VkBuffer* buffer, VkMemoryPropertyFlags flags, bool alone) {
	VkResult result;
	if (info != nullptr) {
		result = vkCreateBuffer(m_device, info, nullptr, buffer);
		if (result != VK_SUCCESS)
			return result;
	}
	result = m_allocator->allocate(*buffer, flags, alone);
	if (result != VK_SUCCESS && info != nullptr)
		vkDestroyBuffer(m_device, *buffer, nullptr);
	return result;
}

VkResult Device::create(VkImageCreateInfo* info, VkImage *image, VkMemoryPropertyFlags flags, bool alone) {
	VkResult result;
	if (info != nullptr) {
		result = vkCreateImage(m_device, info, nullptr, image);
		if (result != VK_SUCCESS)
			return result;
	}
	result = m_allocator->allocate(*image, flags, alone);
	if (result != VK_SUCCESS && info != nullptr)
		vkDestroyImage(m_device, *image, nullptr);
	return result;
}

void* Device::map(VkBuffer buffer) {
	return m_allocator->map(buffer);
}

void Device::unmap(VkBuffer buffer) {
	m_allocator->unmap(buffer);
}

void Device::destroy(VkDeviceMemory heap) {
	m_allocator->free(heap);
}

void Device::destroy(VkBuffer buffer) {
	uint32_t id = m_allocator->getAllocationID(buffer);
	vkDestroyBuffer(m_device, buffer, nullptr);
	m_allocator->free(id);
}

void Device::destroy(VkImage image) {
	uint32_t id = m_allocator->getAllocationID(image);
	vkDestroyImage(m_device, image, nullptr);
	m_allocator->free(id);
}