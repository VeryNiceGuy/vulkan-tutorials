#include "swapchain.h"
#include <stdbool.h>

static void createSwapchainImageViews(Swapchain* swapchain) {
    swapchain->swapchainImageViews = malloc(sizeof(VkImageView) * swapchain->swapchainImageCount);
    for (uint32_t i = 0; i < swapchain->swapchainImageCount; ++i) {
        VkImageViewCreateInfo imageViewCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = swapchain->swapchainImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = swapchain->surfaceFormat.format,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };
        vkCreateImageView(swapchain->device, &imageViewCreateInfo, NULL, &swapchain->swapchainImageViews[i]);
    }
}

static void destroyImageViews(VkDevice device, uint32_t count, VkImageView* views) {
    for (uint32_t i = 0; i < count; ++i) {
        vkDestroyImageView(device, views[i], NULL);
    }
    free(views);
}

static void getSwapchainImages(Swapchain* swapchain) {
    vkGetSwapchainImagesKHR(swapchain->device, swapchain->swapchain, &swapchain->swapchainImageCount, NULL);
    swapchain->swapchainImages = malloc(sizeof(VkImage) * swapchain->swapchainImageCount);
    vkGetSwapchainImagesKHR(swapchain->device, swapchain->swapchain, &swapchain->swapchainImageCount, swapchain->swapchainImages);
}

static void adjustImageExtent(Swapchain* swapchain, VkSurfaceCapabilitiesKHR* capabilities) {
    if (capabilities->currentExtent.width == UINT32_MAX) {
        if (swapchain->imageExtent.width < capabilities->minImageExtent.width) {
            swapchain->imageExtent.width = capabilities->minImageExtent.width;
        }
        else if (swapchain->imageExtent.width > capabilities->maxImageExtent.width) {
            swapchain->imageExtent.width = capabilities->maxImageExtent.width;
        }

        if (swapchain->imageExtent.height < capabilities->minImageExtent.height) {
            swapchain->imageExtent.height = capabilities->minImageExtent.height;
        }
        else if (swapchain->imageExtent.height > capabilities->maxImageExtent.height) {
            swapchain->imageExtent.height = capabilities->maxImageExtent.height;
        }
    }
    else {
        swapchain->imageExtent = capabilities->currentExtent;
    }
}

static bool isSurfaceFormatSupported(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    VkSurfaceFormatKHR* surfaceFormat) {

    uint32_t surfaceFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, NULL);

    VkSurfaceFormatKHR* surfaceFormats = malloc(sizeof(VkSurfaceFormatKHR) * surfaceFormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, surfaceFormats);

    bool result = false;
    for (uint32_t i = 0; i < surfaceFormatCount; ++i) {
        if (surfaceFormats[i].format == surfaceFormat->format && surfaceFormats[i].colorSpace == surfaceFormat->colorSpace) {
            result = true;
            break;
        }
    }

    free(surfaceFormats);
    return result;
}

static bool isPresentModeSupported(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    VkPresentModeKHR presentMode) {

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, NULL);

    VkPresentModeKHR* presentModes = malloc(sizeof(VkPresentModeKHR) * presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes);

    bool result = false;
    for (uint32_t i = 0; i < presentModeCount; ++i) {
        if (presentModes[i] == presentMode) {
            result = true;
        }
    }
    free(presentModes);
    return result;
}

void createSwapchain2(
    VkPhysicalDevice physicalDevice,
    VkDevice device,
    HWND hWnd,
    VkSurfaceKHR surface,
    uint32_t width,
    uint32_t height,
    uint32_t graphicsQueueFamilyIndex,
    uint32_t presentQueueFamilyIndex,
    Swapchain* swapchain) {

    swapchain->physicalDevice = physicalDevice;
    swapchain->device = device;
    swapchain->hWnd = hWnd;
    swapchain->graphicsQueueFamilyIndex = graphicsQueueFamilyIndex;
    swapchain->presentQueueFamilyIndex = presentQueueFamilyIndex;
    swapchain->surface = surface;

    swapchain->surfaceFormat = (VkSurfaceFormatKHR){ .format = VK_FORMAT_B8G8R8A8_SRGB, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
    isSurfaceFormatSupported(swapchain->physicalDevice, swapchain->surface, &swapchain->surfaceFormat);

    VkPresentModeKHR presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    isPresentModeSupported(swapchain->physicalDevice, swapchain->surface, presentMode);

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);
    adjustImageExtent(swapchain, &capabilities);

    VkSurfaceFullScreenExclusiveWin32InfoEXT fullscreenExclusiveInfo = {
        .sType = VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_WIN32_INFO_EXT,
        .pNext = NULL,
        .hmonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST)
    };

    VkSurfaceFullScreenExclusiveInfoEXT surfaceFullscreenExclusiveInfo = {
        .sType = VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_INFO_EXT,
        .pNext = &fullscreenExclusiveInfo,
        .fullScreenExclusive = VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT
    };

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = &surfaceFullscreenExclusiveInfo,
        .surface = surface,
        .minImageCount = capabilities.minImageCount,
        .imageFormat = swapchain->surfaceFormat.format,
        .imageColorSpace = swapchain->surfaceFormat.colorSpace,
        .imageExtent = swapchain->imageExtent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = NULL,
        .pQueueFamilyIndices = NULL,
        .queueFamilyIndexCount = 0
    };

    uint32_t queueFamilyIndices[] = { graphicsQueueFamilyIndex, presentQueueFamilyIndex };

    if (graphicsQueueFamilyIndex != presentQueueFamilyIndex) {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    vkCreateSwapchainKHR(device, &swapchainCreateInfo, NULL, &swapchain->swapchain);
    getSwapchainImages(swapchain);
    createSwapchainImageViews(swapchain);
}

void recreateSwapchain(Swapchain* swapchain) {
    swapchain->surfaceFormat = (VkSurfaceFormatKHR){ .format = VK_FORMAT_B8G8R8A8_SRGB, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
    isSurfaceFormatSupported(swapchain->physicalDevice, swapchain->surface, &swapchain->surfaceFormat);

    VkPresentModeKHR presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    isPresentModeSupported(swapchain->physicalDevice, swapchain->surface, presentMode);

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(swapchain->physicalDevice, swapchain->surface, &capabilities);
    adjustImageExtent(swapchain, &capabilities);

    VkSwapchainKHR oldSwapchain = swapchain->swapchain;
    uint32_t oldSwapchainImageCount = swapchain->swapchainImageCount;
    VkImageView* oldSwapchainImageViews = swapchain->swapchainImageViews;

    VkSurfaceFullScreenExclusiveWin32InfoEXT fullscreenExclusiveInfo = {
        .sType = VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_WIN32_INFO_EXT,
        .pNext = NULL,
        .hmonitor = MonitorFromWindow(swapchain->hWnd, MONITOR_DEFAULTTONEAREST)
    };

    VkSurfaceFullScreenExclusiveInfoEXT surfaceFullscreenExclusiveInfo = {
        .sType = VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_INFO_EXT,
        .pNext = &fullscreenExclusiveInfo,
        .fullScreenExclusive = VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT
    };

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = &surfaceFullscreenExclusiveInfo,
        .surface = swapchain->surface,
        .minImageCount = capabilities.minImageCount,
        .imageFormat = swapchain->surfaceFormat.format,
        .imageColorSpace = swapchain->surfaceFormat.colorSpace,
        .imageExtent = swapchain->imageExtent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = swapchain->swapchain,
        .pQueueFamilyIndices = NULL,
        .queueFamilyIndexCount = 0
    };

    uint32_t queueFamilyIndices[] = { swapchain->graphicsQueueFamilyIndex, swapchain->presentQueueFamilyIndex };

    if (swapchain->graphicsQueueFamilyIndex != swapchain->presentQueueFamilyIndex) {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    vkCreateSwapchainKHR(swapchain->device, &swapchainCreateInfo, NULL, &swapchain->swapchain);
    getSwapchainImages(swapchain);
    createSwapchainImageViews(swapchain);
    destroyImageViews(swapchain->device, oldSwapchainImageCount, oldSwapchainImageViews);
    vkDestroySwapchainKHR(swapchain->device, oldSwapchain, NULL);
}

void destroySwapchain2(Swapchain* swapchain) {
    destroyImageViews(swapchain->device, swapchain->swapchainImageCount, swapchain->swapchainImageViews);
    vkDestroySwapchainKHR(swapchain->device, swapchain->swapchain, NULL);
}