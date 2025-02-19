#pragma once
#include <vulkan/vulkan.h>

void createDescriptorPool(
    VkDevice device,
    uint32_t count,
    VkDescriptorPoolSize* sizes,
    uint32_t maxSets,
    VkDescriptorPool* pool
);