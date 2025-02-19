#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

typedef struct Swapchain {
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    uint32_t graphicsQueueFamilyIndex;
    uint32_t presentQueueFamilyIndex;
    HWND hWnd;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;
    VkSurfaceFormatKHR surfaceFormat;
    VkExtent2D imageExtent;
    uint32_t swapchainImageCount;
    VkImage* swapchainImages;
    VkImageView* swapchainImageViews;
} Swapchain;

void createSwapchain(
    VkPhysicalDevice physicalDevice,
    VkDevice device,
    HWND hWnd,
    VkSurfaceKHR surface,
    uint32_t width,
    uint32_t height,
    uint32_t graphicsQueueFamilyIndex,
    uint32_t presentQueueFamilyIndex,
    Swapchain* swapchain
);

void recreateSwapchain(Swapchain* swapchain);
void destroySwapchain(Swapchain* swapchain);
void enterFullscreenMode(Swapchain* swapchain);