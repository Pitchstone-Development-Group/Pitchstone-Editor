#include "Window.hpp"
#include <vector>
#include <algorithm>
#include <iostream>

Window::Window(Vulkan *vulkan, int width, int height, std::string title) {
	m_window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
	m_closed = m_hidden = m_maximized = false;
	
	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1);
	m_width = width, m_height = height;
	m_vulkan = vulkan;
	VkResult ret = glfwCreateWindowSurface(m_vulkan->m_instance, m_window, nullptr, &m_surface);
	if (ret != VK_SUCCESS)
		throw std::runtime_error("Failed to create window surface! Code: " + std::to_string((int)ret));
}

Window::~Window() {
	vkDeviceWaitIdle(m_device->m_device);
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	ImGui_ImplVulkanH_DestroyWindow(m_vulkan->m_instance, m_device->m_device, &m_imgui, nullptr);
	vkDestroyDescriptorPool(m_device->m_device, m_descriptorPool, nullptr);
	glfwDestroyWindow(m_window);
	m_window = NULL;
}

void Window::update() {
	glfwPollEvents();
	if (m_rebuild) {
		int width, height;
		glfwGetFramebufferSize(m_window, &width, &height);
		if (width > 0 && height > 0) {
			ImGui_ImplVulkan_SetMinImageCount(2);
			ImGui_ImplVulkanH_CreateOrResizeWindow(m_vulkan->m_instance, m_device->m_physical, m_device->m_device, &m_imgui, m_queue->m_family, nullptr, width, height, 2);
			m_imgui.FrameIndex = 0;
			m_rebuild = false;
		}
	}
}

void Window::draw(ImDrawData* drawData) {
	ImVec4 clear_color = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
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

void Window::setupImgui(Device* device, Queue* wsi) {
	m_device = device;
	m_queue = wsi;
	VkDescriptorPoolSize pool_sizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 }, 
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	};
	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
	pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;
	vkCreateDescriptorPool(m_device->m_device, &pool_info, nullptr, &m_descriptorPool);

	int width, height;
	glfwGetFramebufferSize(m_window, &width, &height);
	m_imgui.Surface = m_surface;
	const VkFormat formats[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
	const VkColorSpaceKHR colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	m_imgui.SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(m_device->m_physical, m_imgui.Surface, formats, (size_t)IM_ARRAYSIZE(formats), colorSpace);
	VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
	m_imgui.PresentMode = ImGui_ImplVulkanH_SelectPresentMode(m_device->m_physical, m_imgui.Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));
	ImGui_ImplVulkanH_CreateOrResizeWindow(m_vulkan->m_instance, m_device->m_physical, m_device->m_device, &m_imgui, m_queue->m_family, nullptr, width, height, 2);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForVulkan(m_window, true);
	ImGui_ImplVulkan_InitInfo init_i = {};
	init_i.Instance = m_vulkan->m_instance;
	init_i.PhysicalDevice = m_device->m_physical;
	init_i.Device = m_device->m_device;
	init_i.QueueFamily = m_queue->m_family;
	init_i.Queue = m_queue->m_queue;
	init_i.PipelineCache = m_cache;
	init_i.DescriptorPool = m_descriptorPool;
	init_i.Subpass = 0;
	init_i.MinImageCount = 2;
	init_i.ImageCount = m_imgui.ImageCount;
	init_i.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_i.Allocator = nullptr;
	init_i.CheckVkResultFn = nullptr;
	ImGui_ImplVulkan_Init(&init_i, m_imgui.RenderPass);

	VkCommandPool command_pool = m_imgui.Frames[m_imgui.FrameIndex].CommandPool;
	VkCommandBuffer command_buffer = m_imgui.Frames[m_imgui.FrameIndex].CommandBuffer;

	vkResetCommandPool(m_device->m_device, command_pool, 0);
	VkCommandBufferBeginInfo info_cbb = {};
	info_cbb.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	info_cbb.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(command_buffer, &info_cbb);

	ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

	VkSubmitInfo info_s = {};
	info_s.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	info_s.commandBufferCount = 1;
	info_s.pCommandBuffers = &command_buffer;
	vkEndCommandBuffer(command_buffer);
	vkQueueSubmit(m_queue->m_queue, 1, &info_s, VK_NULL_HANDLE);
	vkDeviceWaitIdle(m_device->m_device);
	ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void Window::render(ImDrawData *drawData) {
	VkResult err;
	VkSemaphore image_acquired_semaphore  = m_imgui.FrameSemaphores[m_imgui.SemaphoreIndex].ImageAcquiredSemaphore;
	VkSemaphore render_complete_semaphore = m_imgui.FrameSemaphores[m_imgui.SemaphoreIndex].RenderCompleteSemaphore;
	err = vkAcquireNextImageKHR(m_device->m_device, m_imgui.Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &m_imgui.FrameIndex);
	if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
	{
		m_rebuild = true;
		return;
	}

	ImGui_ImplVulkanH_Frame* fd = &m_imgui.Frames[m_imgui.FrameIndex];
	vkWaitForFences(m_device->m_device, 1, &fd->Fence, VK_TRUE, UINT64_MAX);
	vkResetFences(m_device->m_device, 1, &fd->Fence);
	vkResetCommandPool(m_device->m_device, fd->CommandPool, 0);

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
	info_s.pWaitSemaphores = &image_acquired_semaphore;
	info_s.pWaitDstStageMask = &wait_stage;
	info_s.commandBufferCount = 1;
	info_s.pCommandBuffers = &fd->CommandBuffer;
	info_s.signalSemaphoreCount = 1;
	info_s.pSignalSemaphores = &render_complete_semaphore;

	vkEndCommandBuffer(fd->CommandBuffer);
	vkQueueSubmit(m_queue->m_queue, 1, &info_s, fd->Fence);

	VkPresentInfoKHR info_p = {};
	info_p.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	info_p.waitSemaphoreCount = 1;
	info_p.pWaitSemaphores = &render_complete_semaphore;
	info_p.swapchainCount = 1;
	info_p.pSwapchains = &m_imgui.Swapchain;
	info_p.pImageIndices = &m_imgui.FrameIndex;
	err = vkQueuePresentKHR(m_queue->m_queue, &info_p);
	if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
		m_rebuild = true;
		return;
	}
	m_imgui.SemaphoreIndex = (m_imgui.SemaphoreIndex + 1) % m_imgui.ImageCount;
}