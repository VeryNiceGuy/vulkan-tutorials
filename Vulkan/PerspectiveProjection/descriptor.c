#include "descriptor.h"

void createDescriptorPool(
    VkDevice device,
    uint32_t count,
    VkDescriptorPoolSize* sizes,
    uint32_t maxSets,
    VkDescriptorPool* pool
) {
    VkDescriptorPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = count,
        .pPoolSizes = sizes,
        .maxSets = maxSets
    };
    vkCreateDescriptorPool(device, &poolInfo, NULL, pool);
}