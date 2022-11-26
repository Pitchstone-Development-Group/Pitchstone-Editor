#ifndef PITCHSTONE_ENGINE_ENGINE
#define PITCHSTONE_ENGINE_ENGINE

#include <mutex>
#include <vulkan/vulkan.h>
#include "../vulkan/Texture.hpp"
#include "../vulkan/Framebuffer.hpp"
#include "../vulkan/Rectangle.hpp"

/* The engine is the core mechanism for grabbing the necessary frames,
 * rendering out the filters, and then return them as a framebuffer to
 * be collected and used as a texture for ImGui. This is used as a soft
 * deadline failure principle where a missed deadline, however unfortunate,
 * will be treated as a degraded output rather than a total failure. 
 */

class Engine {
public:
	Engine(VkDevice device, int width, int height);
	~Engine();

	/* Resize the matierals for use */
	bool proxy(int width, int height);

	/* For ImGui and obseleting redundant frames */
	void swap(Framebuffer **framebuffer);

	std::vector<VkQueue> m_queues; //Dependent on queue family with most number of queues
	VkCommandPool m_pool;
	std::vector<VkCommandBuffer> m_buffers;
	std::vector<VkFence> m_fences;
	Rectangle m_rectangle;

	/* Temporary information, as they should belong in the filter node class */
	std::vector<VkSemaphore> m_semaphore;
	std::vector<VkDescriptorSetLayout> m_desclayouts;
	std::vector<VkDescriptorPool> m_descpools;
	std::vector<VkDescriptorSet> m_descsets;

	std::vector<Texture> m_imports;

	std::mutex m_mutex;
	std::vector<Framebuffer*> m_exports;
	int64_t m_timestamps;

	std::vector<Framebuffer> m_engine;
	VkRenderPass m_renderpass;
};

#endif