#pragma once
#include <vulkan/vulkan.h>

void createTextureImageFromFile(
    char* filename,
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkImage* textureImage,
    VkDeviceMemory* textureImageMemory
);

void createTextureImageView(VkDevice device, VkImage image, VkImageView* view);
void createTextureSampler(VkDevice device, VkSampler* sampler);