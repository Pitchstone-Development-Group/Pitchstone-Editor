#pragma once

#include "Device.hpp"
#include "Buffer.hpp"
#include "CommandPool.hpp"
#include "FiFSyncs.hpp"
#include "Image.hpp"
#include "Timeline.hpp"
#include <imgui/imgui.h>
#include <mutex>

class ImGui_13 {
public:
	ImGui_13(Device *device, VkSurfaceKHR surface, uint32_t width, uint32_t height);
	~ImGui_13();

	VkDescriptorSet add(VkSampler sampler, VkImageView view, VkImageLayout layout);
	void remove(VkDescriptorSet set);


	void draw(ImDrawData *data);
	void present(bool rebuild, uint32_t width, uint32_t height);
private:
	void createOrResizeSwapchain(uint32_t width, uint32_t height);
	void destroySwapchain();

	void createImages(uint32_t width, uint32_t height);
	void resizeImages(int index, uint32_t width, uint32_t height);
	void destroyImages();

	void createFonts();
	void destroyFonts();

	void createPipeline();
	void destroyPipeline();

	void createSyncs();
	void destroySyncs();

	Device *m_device;
	Queue *m_queueWindow, *m_queuePresent;
	VkDescriptorSetLayout m_dslayout;
	VkPipelineLayout m_pllayout;
	VkPipeline m_pipeline;
	VkDescriptorPool m_pool;
	VkDescriptorSet m_sets;

	PFN_vkSetDeviceMemoryPriorityEXT m_deviceMemoryPriority;

	int m_index;
	struct {
		Image *image;
		Buffer *vertices;
		Buffer *indices;
		CommandPool *pool;
	} m_images[2];

	struct {
		VkSampler sampler;
		Image *image;
		VkDescriptorSet set;
	} m_font;

	struct {
		uint64_t waitDraw;
		uint64_t waitBlit;
		uint64_t waitPresent;
		Timeline *semaphore;
	} m_timeline;

	struct {
		bool rebuild;
		uint32_t width, height;
		VkSurfaceKHR surface;
		VkSurfaceFormatKHR format;
		VkSwapchainKHR swapchain;
		std::vector<VkImage> images;
		std::vector<VkImageView> views;
		FiFSyncs *fif;
	} m_window;
};
