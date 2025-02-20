#include "buffer.h"
#include "memory.h"
#include <string.h>

void createVertexBuffer(
    VkPhysicalDevice physicalDevice,
    VkDevice device,
    Buffer* buffer,
    size_t data_size,
    void* data_ptr) {

    buffer->device = device;

    VkBufferCreateInfo vertexBufferInfoCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = data_size,
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };
    vkCreateBuffer(device, &vertexBufferInfoCreateInfo, NULL, &buffer->buffer);

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, buffer->buffer, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = findMemoryType(
            physicalDevice,
            memoryRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
    };
    vkAllocateMemory(device, &memoryAllocateInfo, NULL, &buffer->memory);
    vkBindBufferMemory(device, buffer->buffer, buffer->memory, 0);

    void* data;
    vkMapMemory(device, buffer->memory, 0, data_size, 0, &data);
    memcpy(data, data_ptr, data_size);
    vkUnmapMemory(device, buffer->memory);
}

void createIndexBuffer(
    VkPhysicalDevice physicalDevice,
    VkDevice device,
    Buffer* buffer,
    size_t data_size,
    void* data_ptr) {

    buffer->device = device;

    VkBufferCreateInfo indexBufferCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = data_size,
        .usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    vkCreateBuffer(device, &indexBufferCreateInfo, NULL, &buffer->buffer);

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, buffer->buffer, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = findMemoryType(
            physicalDevice,
            memoryRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
    };
    vkAllocateMemory(device, &memoryAllocateInfo, NULL, &buffer->memory);
    vkBindBufferMemory(device, buffer->buffer, buffer->memory, 0);

    void* data;
    vkMapMemory(device, buffer->memory, 0, data_size, 0, &data);
    memcpy(data, data_ptr, data_size);
    vkUnmapMemory(device, buffer->memory);
}

void createUniformBuffer(
    VkPhysicalDevice physicalDevice,
    VkDevice device,
    Buffer* buffer,
    size_t buffer_size) {

    buffer->device = device;

    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = buffer_size,
        .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    vkCreateBuffer(device, &bufferInfo, NULL, &buffer->buffer);

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer->buffer, &memRequirements);

    VkMemoryAllocateInfo memoryAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = findMemoryType(
            physicalDevice,
            memRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
    };

    vkAllocateMemory(device, &memoryAllocateInfo, NULL, &buffer->memory);
    vkBindBufferMemory(device, buffer->buffer, buffer->memory, 0);
}

void destroyBuffer(Buffer* buffer) {
    vkDestroyBuffer(buffer->device, buffer->buffer, NULL);
    vkFreeMemory(buffer->device, buffer->memory, NULL);
}