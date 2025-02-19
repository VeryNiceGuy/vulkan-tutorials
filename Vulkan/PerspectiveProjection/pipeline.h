#pragma once
#include <vulkan/vulkan.h>

typedef struct Pipeline {
	VkPipeline pipeline;
	VkPipelineLayout layout;
	VkPipelineCache cache;
} Pipeline;

void createPipeline(
    VkDevice device,
    uint32_t descriptorSetLayoutCount,
    VkDescriptorSetLayout* descriptorSetLayouts,
    VkRenderPass renderPass,
    Pipeline* pipeline
);