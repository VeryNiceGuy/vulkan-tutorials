#pragma once
#include <vulkan/vulkan.h>

VkDebugUtilsMessengerCreateInfoEXT createDebugMessengerCreateInfo();
VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger
);
