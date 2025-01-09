#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

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