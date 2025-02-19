#pragma once
#include <vulkan/vulkan.h>

VkShaderModule createShaderModule(VkDevice device, const char* filename);