#include "Window.hpp"
#include <SDL2/SDL_vulkan.h>
#include <imgui/imgui_impl_sdl2.h>
#include <iostream>

Window::Window(Instance *instance, int width, int height, const std::string& title) {
	m_instance = instance->instance();
	m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);
	SDL_Vulkan_CreateSurface(m_window, m_instance, &m_surface);
	SDL_SetWindowData(m_window, "this", this);
}

Window::~Window() {
	vkDeviceWaitIdle(m_device->device());
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	ImGui_ImplVulkanH_DestroyWindow(m_instance, m_device->device(), &m_imgui, VK_NULL_HANDLE);
	vkDestroyDescriptorPool(m_device->device(), m_descriptorPool, nullptr);
	SDL_DestroyWindow(m_window);
}

void Window::draw() {
	ImVec4 clear_color = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	int width, height;
	SDL_GetWindowSize(m_window, &width, &height);
	
	ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

	ImGui::ShowDemoWindow();

	ImGui::Render();
    ImDrawData* drawData = ImGui::GetDrawData();

	const bool is_minimized = (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f);
		if (!is_minimized)
		{
			m_imgui.ClearValue.color.float32[0] = clear_color.x * clear_color.w;
			m_imgui.ClearValue.color.float32[1] = clear_color.y * clear_color.w;
			m_imgui.ClearValue.color.float32[2] = clear_color.z * clear_color.w;
			m_imgui.ClearValue.color.float32[3] = clear_color.w;
			render(drawData);
		}
}

void Window::setupImgui(Device *device) {
	m_device = device;
	m_queue = device->queue();

	// Will need to tweak over time
	uint32_t maxSizePerType = 1000;
	std::vector<VkDescriptorPoolSize> poolSizes =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, maxSizePerType },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, maxSizePerType },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, maxSizePerType },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, maxSizePerType },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, maxSizePerType },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, maxSizePerType }, 
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, maxSizePerType },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, maxSizePerType },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, maxSizePerType },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, maxSizePerType },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, maxSizePerType }
	};
	VkDescriptorPoolCreateInfo infoPool = {};
	infoPool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	infoPool.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	infoPool.maxSets = maxSizePerType * (uint32_t) poolSizes.size();
	infoPool.poolSizeCount = (uint32_t) poolSizes.size();
	infoPool.pPoolSizes = poolSizes.data();
	vkCreateDescriptorPool(m_device->device(), &infoPool, VK_NULL_HANDLE, &m_descriptorPool);

	m_imgui = {};
	int width, height;
	SDL_GetWindowSize(m_window, &width, &height);
	m_imgui.Surface = m_surface;
	std::vector<VkFormat> wantedFormats = { VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_R8G8B8A8_SRGB, VK_FORMAT_B8G8R8_SRGB, VK_FORMAT_R8G8B8_SRGB };
   	VkColorSpaceKHR wantedColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    m_imgui.SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(m_device->physical(), m_imgui.Surface, wantedFormats.data(), (int)wantedFormats.size(), wantedColorSpace);
	std::vector<VkPresentModeKHR> wantedPresentModes = { VK_PRESENT_MODE_FIFO_KHR };
	m_imgui.PresentMode = ImGui_ImplVulkanH_SelectPresentMode(m_device->physical(), m_imgui.Surface, wantedPresentModes.data(), (int)wantedPresentModes.size());
	ImGui_ImplVulkanH_CreateOrResizeWindow(m_instance, m_device->physical(), m_device->device(), &m_imgui, 0, VK_NULL_HANDLE, width, height, 2);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForVulkan(m_window);
	ImGui_ImplVulkan_InitInfo init_i{};
   	init_i.Instance = m_instance;
	init_i.PhysicalDevice = m_device->physical();
	init_i.Device = m_device->device();
	init_i.QueueFamily = 0;
	init_i.Queue = m_queue;
	init_i.PipelineCache = m_cache;
	init_i.DescriptorPool = m_descriptorPool;
	init_i.Subpass = 0;
	init_i.MinImageCount = 2;
	init_i.ImageCount = m_imgui.ImageCount;
	init_i.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_i.Allocator = VK_NULL_HANDLE;
	init_i.CheckVkResultFn = VK_NULL_HANDLE;
    ImGui_ImplVulkan_Init(&init_i, m_imgui.RenderPass);
}

void Window::render(ImDrawData *drawData) {
	VkResult err;
	VkSemaphore acquired  = m_imgui.FrameSemaphores[m_imgui.SemaphoreIndex].ImageAcquiredSemaphore;
	VkSemaphore rendered = m_imgui.FrameSemaphores[m_imgui.SemaphoreIndex].RenderCompleteSemaphore;

	err = vkAcquireNextImageKHR(m_device->device(), m_imgui.Swapchain, UINT64_MAX, acquired, VK_NULL_HANDLE, &m_imgui.FrameIndex);
	if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
	{
		m_rebuild = true;
		return;
	}

	ImGui_ImplVulkanH_Frame* fd = &m_imgui.Frames[m_imgui.FrameIndex];
	vkWaitForFences(m_device->device(), 1, &fd->Fence, VK_TRUE, UINT64_MAX);
	vkResetFences(m_device->device(), 1, &fd->Fence);
	vkResetCommandPool(m_device->device(), fd->CommandPool, 0);

	VkCommandBufferBeginInfo info_ccb = {};
	info_ccb.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	info_ccb.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(fd->CommandBuffer, &info_ccb);

	VkRenderPassBeginInfo info_rpb = {};
	info_rpb.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	info_rpb.renderPass = m_imgui.RenderPass;
	info_rpb.framebuffer = fd->Framebuffer;
	info_rpb.renderArea.extent.width = m_imgui.Width;
	info_rpb.renderArea.extent.height = m_imgui.Height;
	info_rpb.clearValueCount = 1;
	info_rpb.pClearValues = &m_imgui.ClearValue;
	vkCmdBeginRenderPass(fd->CommandBuffer, &info_rpb, VK_SUBPASS_CONTENTS_INLINE);

	ImGui_ImplVulkan_RenderDrawData(drawData, fd->CommandBuffer);

	vkCmdEndRenderPass(fd->CommandBuffer);
	VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo info_s = {};
	info_s.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	info_s.waitSemaphoreCount = 1;
	info_s.pWaitSemaphores = &acquired;
	info_s.pWaitDstStageMask = &wait_stage;
	info_s.commandBufferCount = 1;
	info_s.pCommandBuffers = &fd->CommandBuffer;
	info_s.signalSemaphoreCount = 1;
	info_s.pSignalSemaphores = &rendered;

	vkEndCommandBuffer(fd->CommandBuffer);
	vkQueueSubmit(m_queue, 1, &info_s, fd->Fence);

	VkPresentInfoKHR info_p = {};
	info_p.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	info_p.waitSemaphoreCount = 1;
	info_p.pWaitSemaphores = &rendered;
	info_p.swapchainCount = 1;
	info_p.pSwapchains = &m_imgui.Swapchain;
	info_p.pImageIndices = &m_imgui.FrameIndex;
	err = vkQueuePresentKHR(m_queue, &info_p);
	if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
		m_rebuild = true;
		return;
	}
	m_imgui.SemaphoreIndex = (m_imgui.SemaphoreIndex + 1) % m_imgui.SemaphoreCount;
}

