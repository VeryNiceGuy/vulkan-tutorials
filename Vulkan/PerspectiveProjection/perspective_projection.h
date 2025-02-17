#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "matrix4x4.h"

typedef struct SceneObject {
	Matrix4x4 world;
	VkBuffer vertex_buffer;
	VkDeviceMemory vertex_buffer_memory;
	VkBuffer index_buffer;
	VkDeviceMemory index_buffer_memory;
} SceneObject;

static const uint32_t WIDTH = 800;
static const uint32_t HEIGHT = 600;

void initialize(HINSTANCE hInstance, HWND hWnd);
void render();
void deinitialize();

void moveForward();
void moveLeft();
void moveRight();
void moveBackward();
void mouseMove(float x, float y);

void enableFullScreen();