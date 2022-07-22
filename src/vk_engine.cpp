﻿
#include "vk_engine.h"

#include <SDL.h>
#include <SDL_vulkan.h>

#include <vk_types.h>
#include <vk_initializers.h>

// bootstrap library
#include "VkBootstrap.h"

#include <iostream>

//we want to immediately abort when there is an error. In normal engines this would give an error message to the user, or perform a dump of state.
using namespace std;
#define VK_CHECK(x)                                                 \
	do                                                              \
	{                                                               \
		VkResult err = x;                                           \
		if (err)                                                    \
		{                                                           \
			std::cout <<"Detected Vulkan error: " << err << std::endl; \
			abort();                                                \
		}                                                           \
	} while (0)

void VulkanEngine::init()
{
	// We initialize SDL and create a window with it. 
	SDL_Init(SDL_INIT_VIDEO);

	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);
	
	_window = SDL_CreateWindow(
		"Vulkan Engine",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		_windowExtent.width,
		_windowExtent.height,
		window_flags
	);

	init_vulkan();
	init_swapchain();
	init_commands();
	
	//everything went fine
	_isInitialized = true;
}
void VulkanEngine::cleanup()
{	
	// We tend to destroy things in the reverse order of which they were created
	if (_isInitialized) {
		vkDestroyCommandPool(_device, _commandPool, nullptr);
		vkDestroySwapchainKHR(_device, _swapchain, nullptr);

		// Destroy swapchain resources
		for (int i = 0; i < _swapchainImageViews.size(); i++)
		{
			vkDestroyImageView(_device, _swapchainImageViews[i], nullptr);
		}

		vkDestroyDevice(_device, nullptr);
		vkDestroySurfaceKHR(_instance, _surface, nullptr);
		vkb::destroy_debug_utils_messenger(_instance, _debug_messenger);
		vkDestroyInstance(_instance, nullptr);

		SDL_DestroyWindow(_window);
	}
}

void VulkanEngine::draw()
{
	//nothing yet
}

void VulkanEngine::run()
{
	SDL_Event e;
	bool bQuit = false;

	//main loop
	while (!bQuit)
	{
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			//close the window when user alt-f4s or clicks the X button			
			if (e.type == SDL_QUIT) bQuit = true;
		}

		draw();
	}
}

void VulkanEngine::init_vulkan()
{
	// Build a VulkanInstance and store the debug messenger
	vkb::InstanceBuilder builder;
	auto inst_ret = builder.set_app_name("Vulkan Guide Engine")
		.request_validation_layers() // Basic debug features
		.require_api_version(1, 1, 0)
		.use_default_debug_messenger()
		.build();
	vkb::Instance vkb_inst = inst_ret.value();
	_instance = vkb_inst.instance;
	_debug_messenger = vkb_inst.debug_messenger;

	// Create surface from SDL window and create device from selected physical device
	SDL_Vulkan_CreateSurface(_window, _instance, &_surface);

	vkb::PhysicalDeviceSelector selector{ vkb_inst };
	vkb::PhysicalDevice physicalDevice = selector
		.set_minimum_version(1, 1)
		.set_surface(_surface)
		.select()
		.value();
	_chosenGPU = physicalDevice.physical_device;

	vkb::DeviceBuilder deviceBuilder{ physicalDevice };
	vkb::Device vkbDevice = deviceBuilder.build().value();
	_device = vkbDevice.device;

	// Use Vkbootstrap to get a graphics queue
	_graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
	_graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
}


void VulkanEngine::init_swapchain()
{
	// Create the swapchain and its associated structures
	vkb::SwapchainBuilder swapchainBuilder{ _chosenGPU, _device, _surface };
	vkb::Swapchain vkbSwapchain = swapchainBuilder
		.use_default_format_selection()
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR) // vsync
		.set_desired_extent(_windowExtent.width, _windowExtent.height) // If we resize the window, need to rebuild swapchain
		.build()
		.value();

	_swapchain = vkbSwapchain.swapchain;
	_swapchainImageFormat = vkbSwapchain.image_format;
	_swapchainImages = vkbSwapchain.get_images().value();
	_swapchainImageViews = vkbSwapchain.get_image_views().value();
}

void VulkanEngine::init_commands()
{
	// Create a command pool for commands submitted to the graphics queue
	// and allow resetting of individual command buffers
	VkCommandPoolCreateInfo commandPoolInfo = vkinit::command_pool_create_info(_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	VK_CHECK(vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_commandPool));

	// Allocate a single default command buffer we will use for rendering
	// and indicate it as the Primary command buffer
	VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(_commandPool, 1);

	VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo, &_mainCommandBuffer));
}
