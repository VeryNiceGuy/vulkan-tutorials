#include "texture.h"
#include "tga.h"
#include "memory.h"

void createTextureImageFromFile(
    char* filename,
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkImage* textureImage,
    VkDeviceMemory* textureImageMemory
) {
    TGAImage* image = loadTGA(filename);
    VkDeviceSize imageSize = image->width * image->height * 4;

    VkImageCreateInfo imageInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .extent = {.width = 1024, .height = 1024, .depth = 1 },
        .mipLevels = 1,
        .arrayLayers = 1,
        .format = VK_FORMAT_R8G8B8A8_UNORM,
        .tiling = VK_IMAGE_TILING_LINEAR,
        .initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        .usage = VK_IMAGE_USAGE_SAMPLED_BIT,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    vkCreateImage(device, &imageInfo, NULL, textureImage);

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, *textureImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo = { 0 };
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(
        physicalDevice,
        memRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    vkAllocateMemory(device, &allocInfo, NULL, textureImageMemory);
    vkBindImageMemory(device, *textureImage, *textureImageMemory, 0);

    void* data;
    vkMapMemory(device, *textureImageMemory, 0, VK_WHOLE_SIZE, 0, &data);
    memcpy(data, image->data, (size_t)imageSize);
    vkUnmapMemory(device, *textureImageMemory);

    freeTGA(image);
}

void createTextureImageView(VkDevice device, VkImage image, VkImageView* view) {
    VkImageViewCreateInfo viewInfo = { 0 };
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
    vkCreateImageView(device, &viewInfo, NULL, view);
}

void createTextureSampler(VkDevice device, VkSampler* sampler) {
    VkSamplerCreateInfo samplerInfo = { 0 };
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    vkCreateSampler(device, &samplerInfo, NULL, sampler);
}