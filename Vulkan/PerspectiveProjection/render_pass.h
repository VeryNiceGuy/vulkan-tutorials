#pragma once
#include <vulkan/vulkan.h>

void createRenderPass(
	VkDevice device,
	VkFormat format,
	VkRenderPass* renderPass
);

void destroyRenderPass(VkDevice device, VkRenderPass renderPass);