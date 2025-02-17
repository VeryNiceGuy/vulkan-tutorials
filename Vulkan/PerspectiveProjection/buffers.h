#pragma once
#include<vulkan/vulkan.h>

void createIndexBuffer(
    VkPhysicalDevice physicalDevice,
    VkDevice device,
    VkBuffer* buffer,
    VkDeviceMemory* device_memory,
    size_t data_size,
    void* data_ptr
);

void createVertexBuffer(
    VkPhysicalDevice physicalDevice,
    VkDevice device,
    VkBuffer* buffer,
    VkDeviceMemory* device_memory,
    size_t data_size,
    void* data_ptr
);

void createUniformBuffer(
    VkPhysicalDevice physicalDevice,
    VkDevice device,
    VkBuffer* buffer,
    VkDeviceMemory* device_memory,
    size_t buffer_size
);
