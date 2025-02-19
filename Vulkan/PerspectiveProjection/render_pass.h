#pragma once
#include <vulkan/vulkan.h>

void createRenderPass(
	VkDevice device,
	VkFormat format,
	VkRenderPass* renderPass
);