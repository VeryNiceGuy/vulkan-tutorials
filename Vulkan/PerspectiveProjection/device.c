#include "device.h"
#include <stdbool.h>
#include <stdlib.h>

void getPhysicalDevice(
    VkInstance instance,
    VkPhysicalDevice* physicalDevice
) {
    int32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, NULL);

    VkPhysicalDevice* physicalDevices = malloc(sizeof(VkPhysicalDevice) * physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices);

    *physicalDevice = physicalDevices[1];
    free(physicalDevices);
}

void getQueueFamilies(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    uint32_t* graphicsQueueFamilyIndex,
    uint32_t* presentQueueFamilyIndex
) {
    uint32_t queueFamilyPropertyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, NULL);

    VkQueueFamilyProperties* queueFamilyProperties = malloc(sizeof(VkQueueFamilyProperties) * queueFamilyPropertyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, queueFamilyProperties);

    bool graphicsQueueFamily = false;
    bool presentQueueFamily = false;

    for (uint32_t i = 0; i < queueFamilyPropertyCount; ++i) {
        if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            *graphicsQueueFamilyIndex = i;
            graphicsQueueFamily = true;
        }

        VkBool32 presentSupport;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

        if (presentSupport) {
            *presentQueueFamilyIndex = i;
            presentQueueFamily = true;
        }

        if (graphicsQueueFamily && presentQueueFamily) {
            break;
        }
    }

    free(queueFamilyProperties);
}

void createLogicalDevice(
    VkPhysicalDevice physicalDevice,
    uint32_t graphicsQueueFamilyIndex,
    uint32_t presentQueueFamilyIndex,
    VkDevice* device,
    VkQueue* graphicsQueue,
    VkQueue* presentQueue
) {
    const char* deviceExtensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME,
        VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME
    };

    float queuePriority = 1.0f;

    VkDeviceQueueCreateInfo deviceQueueCreateInfos[2] = { {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = graphicsQueueFamilyIndex,
            .queueCount = 1,
            .pQueuePriorities = &queuePriority
        }, {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = presentQueueFamilyIndex,
            .queueCount = 1,
            .pQueuePriorities = &queuePriority
        }
    };

    VkPhysicalDeviceDescriptorIndexingFeatures indexingFeatures = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES,
        .descriptorBindingPartiallyBound = VK_TRUE
    };

    VkPhysicalDeviceFeatures2 deviceFeatures2 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &indexingFeatures
    };

    VkDeviceCreateInfo deviceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &deviceFeatures2,
        .pQueueCreateInfos = deviceQueueCreateInfos,
        .queueCreateInfoCount = graphicsQueueFamilyIndex != presentQueueFamilyIndex ? 2 : 1,
        .ppEnabledExtensionNames = deviceExtensions,
        .enabledExtensionCount = sizeof(deviceExtensions) / sizeof(deviceExtensions[0])
    };

    vkCreateDevice(physicalDevice, &deviceCreateInfo, NULL, device);
    vkGetDeviceQueue(*device, graphicsQueueFamilyIndex, 0, graphicsQueue);

    if (graphicsQueueFamilyIndex != presentQueueFamilyIndex) {
        vkGetDeviceQueue(*device, presentQueueFamilyIndex, 0, presentQueue);
    }
    else {
        *presentQueue = *graphicsQueue;
    }
}

void destroyLogicalDevice(VkDevice device) {
    vkDestroyDevice(device, NULL);
}