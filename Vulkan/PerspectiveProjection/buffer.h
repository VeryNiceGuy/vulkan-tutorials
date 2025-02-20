#pragma once
#include<vulkan/vulkan.h>

typedef struct Buffer {
    VkBuffer buffer;
    VkDeviceMemory memory;
    VkDevice device;
} Buffer;

void createIndexBuffer(
    VkPhysicalDevice physicalDevice,
    VkDevice device,
    Buffer* buffer,
    size_t data_size,
    void* data_ptr
);

void createVertexBuffer(
    VkPhysicalDevice physicalDevice,
    VkDevice device,
    Buffer* buffer,
    size_t data_size,
    void* data_ptr
);

void createUniformBuffer(
    VkPhysicalDevice physicalDevice,
    VkDevice device,
    Buffer* buffer,
    size_t buffer_size
);

void destroyBuffer(Buffer* buffer);