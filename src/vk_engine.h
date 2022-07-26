﻿// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vk_types.h>
#include <vector>

class VulkanEngine {
public:

	bool _isInitialized{ false };
	int _frameNumber {0};

	VkExtent2D _windowExtent{ 1700 , 900 };

	struct SDL_Window* _window{ nullptr };

	// Core Vulkan structures
	VkInstance _instance; // Vulkan library handle
	VkDebugUtilsMessengerEXT _debug_messenger; // Vulkan debug output handle
	VkPhysicalDevice _chosenGPU; // GPU chosen as the default device
	VkDevice _device; // Vulkan device for commands
	VkSurfaceKHR _surface; // Vulkan window surface

	// Swapchain structures
	VkSwapchainKHR _swapchain;
	VkFormat _swapchainImageFormat;
	std::vector<VkImage> _swapchainImages;
	std::vector<VkImageView> _swapchainImageViews;

	// Queue and command pool/buffer structures
	VkQueue _graphicsQueue;
	uint32_t _graphicsQueueFamily;
	VkCommandPool _commandPool;
	VkCommandBuffer _mainCommandBuffer;

	// Renderpass and framebuffer structures
	VkRenderPass _renderpass;
	std::vector<VkFramebuffer> _framebuffers;


	// Initializes everything in the engine
	void init();

	// Shuts down the engine
	void cleanup();

	// Draw loop
	void draw();

	// Run main loop
	void run();

	// Initialize core Vulkan structures
	void init_vulkan();

	// Initialize swapchain structures
	void init_swapchain();

	// Initialize command structures
	void init_commands();

	// Initialize renderpass
	void init_default_renderpass();

	// Initialize framebuffers
	void init_framebuffers();
};
