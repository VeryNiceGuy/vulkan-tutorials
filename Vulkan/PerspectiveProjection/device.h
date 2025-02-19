#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

void getPhysicalDevice(
    VkInstance instance,
    VkPhysicalDevice* physicalDevice
);

void getQueueFamilies(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    uint32_t* graphicsQueueFamilyIndex,
    uint32_t* presentQueueFamilyIndex
);

void createLogicalDevice(
    VkPhysicalDevice physicalDevice,
    uint32_t graphicsQueueFamilyIndex,
    uint32_t presentQueueFamilyIndex,
    VkDevice* device,
    VkQueue* graphicsQueue,
    VkQueue* presentQueue
);

void destroyLogicalDevice(VkDevice device);