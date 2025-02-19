#include "surface.h"

void createSurface(HINSTANCE hInstance, HWND hWnd, VkInstance instance, VkSurfaceKHR* surface) {
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .pNext = NULL,
        .hinstance = hInstance,
        .hwnd = hWnd
    };
    vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, NULL, surface);
}

void destroySurface(VkSurfaceKHR surface, VkInstance instance) {
    vkDestroySurfaceKHR(instance, surface, NULL);
}
