#pragma once
#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

void createSurface(HINSTANCE hInstance, HWND hWnd, VkInstance instance, VkSurfaceKHR* surface);
void destroySurface(VkSurfaceKHR surface, VkInstance instance);