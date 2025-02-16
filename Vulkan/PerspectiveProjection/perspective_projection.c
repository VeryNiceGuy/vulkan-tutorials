#define _USE_MATH_DEFINES
#include <math.h>

#include "perspective_projection.h"
#include "vector3.h"
#include "matrix4x4.h"
#include "camera.h"
#include "tga.h"

HWND windowHandle;
VkInstance instance;
VkDevice device;
VkPhysicalDevice physicalDevice;
VkSurfaceKHR surface;
VkSwapchainKHR swapchain = VK_NULL_HANDLE;

VkFormat swapChainFormat;
uint32_t swapchainImageCount;
VkImage* swapchainImages;
VkImageView* imageViews;
VkExtent2D imageExtent;

VkRenderPass renderPass;

uint32_t graphicsQueueFamilyIndex;
uint32_t presentQueueFamilyIndex;

VkQueue graphicsQueue;
VkQueue presentQueue;

VkFramebuffer* framebuffers;

VkBuffer vertexBuffer;
VkDeviceMemory vertexBufferMemory;

VkBuffer indexBuffer;
VkDeviceMemory indexBufferMemory;

VkBuffer uniformBuffer;
VkDeviceMemory uniformBufferMemory;

VkCommandPool commandPool;
VkCommandBuffer* commandBuffers;

#define MAX_FRAMES_IN_FLIGHT 2
VkSemaphore imageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT];
VkSemaphore renderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT];
VkFence inFlightFences[MAX_FRAMES_IN_FLIGHT];

VkDeviceMemory uniformBuffersMemory[MAX_FRAMES_IN_FLIGHT];
VkBuffer uniformBuffers[MAX_FRAMES_IN_FLIGHT];

size_t currentFrame = 0;

float width = 800;
float height = 600;

struct Vertex {
    float x, y, z;
    float u, v;
};

struct Vertex vertices[4] = {
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    {20.0f, 0.0f, 0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 20.0f, 0.0f, 1.0f},
    {20.0f, 0.0f, 20.0f, 1.0f, 1.0f}
};


unsigned int indices[6] = {
    0, 1, 2,
    1, 3, 2
};


struct MVP {
    Matrix4x4 model;
    Matrix4x4 view;
    Matrix4x4 proj;
};

struct MVP mvp;
Camera camera;

VkPipelineCache pipelineCache;
VkPipelineLayout pipelineLayout;
VkPipeline pipeline;

VkDescriptorSetLayout descriptorSetLayouts[MAX_FRAMES_IN_FLIGHT];
VkDescriptorPool descriptorPool;
VkDescriptorSet descriptorSets[MAX_FRAMES_IN_FLIGHT];

VkDebugUtilsMessengerEXT debugMessenger;
PFN_vkAcquireFullScreenExclusiveModeEXT vkAcquireFullScreenExclusiveModeEXT2;

VkImage textureImage;
VkDeviceMemory textureImageMemory;
VkImageView textureImageView;
VkSampler textureSampler;
VkDescriptorSet descriptorSet;

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);

    for (uint32_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; ++i) {
        if ((typeFilter & (1 << i)) && (physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
}

/////////////////


void querySupportedFormats() {
    VkFormat formats[] = {
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_FORMAT_B8G8R8A8_UNORM,
        VK_FORMAT_R8G8B8_UNORM,
        VK_FORMAT_B8G8R8_UNORM,
        VK_FORMAT_R8_UNORM,
        VK_FORMAT_D32_SFLOAT,
        // Add more formats as needed
    };

    for (size_t i = 0; i < sizeof(formats) / sizeof(formats[0]); ++i) {
        VkFormat format = formats[i];

        VkPhysicalDeviceImageFormatInfo2 formatInfo = {};
        formatInfo.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2;
        formatInfo.format = format;
        formatInfo.type = VK_IMAGE_TYPE_2D;
        formatInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        formatInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
        formatInfo.flags = 0;

        VkImageFormatProperties2 imageFormatProperties = {};
        imageFormatProperties.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2;

        VkResult result = vkGetPhysicalDeviceImageFormatProperties2(physicalDevice, &formatInfo, &imageFormatProperties);

        if (result == VK_SUCCESS) {
            printf("Format %d is supported.\n", format);
            VkImageFormatProperties properties = imageFormatProperties.imageFormatProperties;
            printf("  Max Extent: %d x %d x %d\n", properties.maxExtent.width, properties.maxExtent.height, properties.maxExtent.depth);
            printf("  Max Mip Levels: %d\n", properties.maxMipLevels);
            printf("  Max Array Layers: %d\n", properties.maxArrayLayers);
            printf("  Sample Counts: %d\n", properties.sampleCounts);
            printf("  Max Resource Size: %llu\n", properties.maxResourceSize);
        }
        else {
            printf("Format %d is not supported.\n", format);
        }
    }
}


////////////////

void createTextureImage() {
    //querySupportedFormats();

    const char* filename = "image.tga";
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

    vkCreateImage(device, &imageInfo, NULL, &textureImage);
    
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, textureImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    vkAllocateMemory(device, &allocInfo, NULL, &textureImageMemory);
    vkBindImageMemory(device, textureImage, textureImageMemory, 0);

    void* data;
    vkMapMemory(device, textureImageMemory, 0, VK_WHOLE_SIZE, 0, &data);
    memcpy(data, image->data, (size_t)imageSize);
    vkUnmapMemory(device, textureImageMemory);

    freeTGA(image);
}

void createTextureImageView() {
    VkImageViewCreateInfo viewInfo = {0};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = textureImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
    vkCreateImageView(device, &viewInfo, NULL, &textureImageView);
}

void createTextureSampler() {
    VkSamplerCreateInfo samplerInfo = {0};
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
    vkCreateSampler(device, &samplerInfo, NULL, &textureSampler);
}

void destroySwapchain() {
    vkDestroySwapchainKHR(device, swapchain, NULL);
    swapchain = VK_NULL_HANDLE;
    free(swapchainImages);
    swapchainImages = NULL;
}

void destroyImageViews() {
    for (uint32_t i = 0; i < swapchainImageCount; ++i) {
        vkDestroyImageView(device, imageViews[i], NULL);
    }
    free(imageViews);
    imageViews = VK_NULL_HANDLE;
}

void destroyFramebuffers() {
    for (uint32_t i = 0; i < swapchainImageCount; ++i) {
        vkDestroyFramebuffer(device, framebuffers[i], NULL);
    }
    free(framebuffers);
    framebuffers = VK_NULL_HANDLE;
}

void destroyCommandBuffers() {
    vkDestroyCommandPool(device, commandPool, NULL);
    free(commandBuffers);
    commandBuffers = VK_NULL_HANDLE;
}

void createImageViews() {
    imageViews = malloc(sizeof(VkImageView) * swapchainImageCount);
    for (uint32_t i = 0; i < swapchainImageCount; ++i) {
        VkImageViewCreateInfo imageViewCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = swapchainImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = swapChainFormat,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };

        vkCreateImageView(device, &imageViewCreateInfo, NULL, &imageViews[i]);
    }
}

void createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding layoutBindings[] = {{
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
    }, {
        .binding = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
    }};

    VkDescriptorSetLayoutCreateInfo layoutInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 2,
        .pBindings = layoutBindings
    };

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        vkCreateDescriptorSetLayout(device, &layoutInfo, NULL, &descriptorSetLayouts[i]);
    }
}

void createDescriptorPool() {
    VkDescriptorPoolSize poolSizes[] = { { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 }, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 } };

    VkDescriptorPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = 2,
        .pPoolSizes = poolSizes,
        .maxSets = MAX_FRAMES_IN_FLIGHT
    };

    vkCreateDescriptorPool(device, &poolInfo, NULL, &descriptorPool);
}

void createDescriptorSet() {
    VkDescriptorSetAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = MAX_FRAMES_IN_FLIGHT,
        .pSetLayouts = descriptorSetLayouts
    };

    vkAllocateDescriptorSets(device, &allocInfo, descriptorSets);

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo bufferInfo = {
            .buffer = uniformBuffers[i],
            .offset = 0,
            .range = VK_WHOLE_SIZE
        };

        VkDescriptorImageInfo imageInfo = {
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .imageView = textureImageView,
            .sampler = textureSampler
        };

        VkWriteDescriptorSet descriptorWrites[] = {{
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptorSets[i],
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .pBufferInfo = &bufferInfo
        }, {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptorSets[i],
            .dstBinding = 1,
            .dstArrayElement = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .pImageInfo = &imageInfo
        }};

        vkUpdateDescriptorSets(device, 2, &descriptorWrites, 0, NULL);
    }
}

void createUniformBuffers() {
    mvp = (struct MVP){
        .model = matrix4x4_create_identity(),
        .view = (Matrix4x4){0},
        .proj = matrix4x4_perspective_fov_lh_gl(0.785398f, 800.0f / 600.0f, 0.1f, 100.0f)
    };

    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = sizeof(mvp),
        .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        vkCreateBuffer(device, &bufferInfo, NULL, &uniformBuffers[i]);

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, uniformBuffers[i], &memRequirements);

        VkMemoryAllocateInfo memoryAllocateInfo = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = memRequirements.size,
            .memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
        };

        vkAllocateMemory(device, &memoryAllocateInfo, NULL, &uniformBuffersMemory[i]);
        vkBindBufferMemory(device, uniformBuffers[i], uniformBuffersMemory[i], 0);
    }
}

void updateUniformBuffer(uint32_t currentFrame) {
    mvp.view = camera_calculate_view_matrix(&camera);

    void* data;
    vkMapMemory(device, uniformBuffersMemory[currentFrame], 0, sizeof(mvp), 0, &data);
    memcpy(data, &mvp, sizeof(mvp));
    vkUnmapMemory(device, uniformBuffersMemory[currentFrame]);
}

void createLogicalDevice() {
    float queuePriority = 1.0f;
    const char* deviceExtensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME
    };

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

    VkDeviceCreateInfo deviceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pQueueCreateInfos = deviceQueueCreateInfos,
        .queueCreateInfoCount = graphicsQueueFamilyIndex != presentQueueFamilyIndex ? 2 : 1,
        .ppEnabledExtensionNames = deviceExtensions,
        .enabledExtensionCount = 2
    };

    VkResult res = vkCreateDevice(physicalDevice, &deviceCreateInfo, NULL, &device);
    vkAcquireFullScreenExclusiveModeEXT2 = (PFN_vkAcquireFullScreenExclusiveModeEXT)vkGetDeviceProcAddr(device, "vkAcquireFullScreenExclusiveModeEXT");

    vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &graphicsQueue);

    if (graphicsQueueFamilyIndex != presentQueueFamilyIndex) {
        vkGetDeviceQueue(device, presentQueueFamilyIndex, 0, &presentQueue);
    }
    else {
        presentQueue = graphicsQueue;
    }
}

void getPhysicalDevice() {
    int32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, NULL);

    VkPhysicalDevice* physicalDevices = malloc(sizeof(VkPhysicalDevice) * physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices);

    physicalDevice = physicalDevices[1];
    free(physicalDevices);
}

void getQueueFamilies() {
    uint32_t queueFamilyPropertyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, NULL);

    VkQueueFamilyProperties* queueFamilyProperties = malloc(sizeof(VkQueueFamilyProperties) * queueFamilyPropertyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, queueFamilyProperties);

    bool graphicsQueueFamily = false;
    bool presentQueueFamily = false;

    for (uint32_t i = 0; i < queueFamilyPropertyCount; ++i) {
        if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsQueueFamilyIndex = i;
        }

        VkBool32 presentSupport;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

        if (presentSupport) {
            presentQueueFamilyIndex = i;
        }

        if (graphicsQueueFamily && presentQueueFamily) {
            break;
        }
    }

    free(queueFamilyProperties);
}

//

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    printf("Validation layer: %s\n", pCallbackData->pMessage);
    return VK_FALSE;
}

VkDebugUtilsMessengerCreateInfoEXT createDebugMessengerCreateInfo() {
    VkDebugUtilsMessengerCreateInfoEXT createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debugCallback
    };

    return createInfo;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != NULL) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void createInstance() {
    VkApplicationInfo applicationInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_0,
    };

    const char* desiredExtensions[] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
        VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME
    };

    const char* validationLayers[] = {
        "VK_LAYER_KHRONOS_validation"
    };

    VkInstanceCreateInfo instanceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &applicationInfo,
        .ppEnabledExtensionNames = desiredExtensions,
        .enabledExtensionCount = 4,
        .ppEnabledLayerNames = validationLayers,
        .enabledLayerCount = 1
    };

    vkCreateInstance(&instanceCreateInfo, NULL, &instance);
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = createDebugMessengerCreateInfo();
    CreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, NULL, &debugMessenger);
}

void createRenderPass() {
    VkAttachmentDescription attachmentDescription = {
        .format = swapChainFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };

    VkAttachmentReference colorAttachmentReference = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkSubpassDescription subpassDescription = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentReference
    };

    VkRenderPassCreateInfo renderPassCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &attachmentDescription,
        .subpassCount = 1,
        .pSubpasses = &subpassDescription
    };

    vkCreateRenderPass(device, &renderPassCreateInfo, NULL, &renderPass);
}

void destroyRenderPass() {
    vkDestroyRenderPass(device, renderPass, NULL);
    renderPass = VK_NULL_HANDLE;
}

void createVertexBuffer() {
    VkBufferCreateInfo vertexBufferInfoCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = sizeof(vertices),
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };
    vkCreateBuffer(device, &vertexBufferInfoCreateInfo, NULL, &vertexBuffer);

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, vertexBuffer, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
    };
    vkAllocateMemory(device, &memoryAllocateInfo, NULL, &vertexBufferMemory);
    vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);

    void* data;
    vkMapMemory(device, vertexBufferMemory, 0, sizeof(vertices), 0, &data);
    memcpy(data, vertices, sizeof(vertices));
    vkUnmapMemory(device, vertexBufferMemory);
}

void createIndexBuffer() {
    VkBufferCreateInfo indexBufferCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = sizeof(indices),
        .usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    vkCreateBuffer(device, &indexBufferCreateInfo, NULL, &indexBuffer);

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, indexBuffer, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
    };
    vkAllocateMemory(device, &memoryAllocateInfo, NULL, &indexBufferMemory);
    vkBindBufferMemory(device, indexBuffer, indexBufferMemory, 0);

    void* data;
    vkMapMemory(device, indexBufferMemory, 0, sizeof(indices), 0, &data);
    memcpy(data, indices, sizeof(indices));
    vkUnmapMemory(device, indexBufferMemory);
}

VkShaderModule createShaderModule(const char* filename) {
    FILE* file = fopen(filename, "rb");
    fseek(file, 0, SEEK_END);
    size_t codeSize = ftell(file);
    rewind(file);

    BYTE* code = malloc(codeSize);
    fread(code, 1, codeSize, file);
    fclose(file);

    VkShaderModuleCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = codeSize,
        .pCode = code
    };

    VkShaderModule shaderModule;
    vkCreateShaderModule(device, &createInfo, NULL, &shaderModule);
    free(code);

    return shaderModule;
}

void createPipeline() {
    VkPipelineShaderStageCreateInfo shaderStageCreateInfos[2] = { {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = createShaderModule("vertex_shader.spv"),
            .pName = "main"
        }, {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = createShaderModule("fragment_shader.spv"),
            .pName = "main"
        }
    };

    VkVertexInputBindingDescription vertexInputBinding = {
        .binding = 0,
        .stride = sizeof(struct Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    };

    VkVertexInputAttributeDescription vertexInputAttributeDescriptions[] = { {
        .binding = 0,
        .location = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = 0
    }, {
        .binding = 0,
        .location = 1,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(struct Vertex, u)
    } };

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &vertexInputBinding,
        .vertexAttributeDescriptionCount = 2,
        .pVertexAttributeDescriptions = &vertexInputAttributeDescriptions
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
    };

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .lineWidth = 1.0f
    };

    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .minSampleShading = 1.0f
    };

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD
    };

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment
    };

    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = imageExtent.width,
        .height = imageExtent.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkRect2D scissor = {
        .offset = { 0, 0 },
        .extent = imageExtent
    };

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor
    };

    VkDynamicState dynamics[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pDynamicStates = dynamics,
        .dynamicStateCount = 2
    };

    VkPipelineLayoutCreateInfo layoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = MAX_FRAMES_IN_FLIGHT,
        .pSetLayouts = descriptorSetLayouts
    };

    vkCreatePipelineLayout(device, &layoutCreateInfo, NULL, &pipelineLayout);

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = shaderStageCreateInfos,
        .pVertexInputState = &vertexInputStateCreateInfo,
        .pInputAssemblyState = &inputAssemblyStateCreateInfo,
        .pViewportState = &viewportStateCreateInfo,
        .pRasterizationState = &rasterizationStateCreateInfo,
        .pMultisampleState = &multisampleStateCreateInfo,
        .pColorBlendState = &colorBlendStateCreateInfo,
        .pDynamicState = &dynamicStateCreateInfo,
        .renderPass = renderPass,
        .layout = pipelineLayout
    };

    VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO
    };

    vkCreatePipelineCache(device, &pipelineCacheCreateInfo, NULL, &pipelineCache);
    vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCreateInfo, NULL, &pipeline);
}

void createSurface(HINSTANCE hInstance, HWND hWnd) {
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .pNext = NULL,
        .hinstance = hInstance,
        .hwnd = hWnd
    };

    vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, NULL, &surface);
}

void destroySurface() {
    vkDestroySurfaceKHR(instance, surface, NULL);
    surface = VK_NULL_HANDLE;
}

void createFramebuffers() {
    framebuffers = malloc(sizeof(VkFramebuffer) * swapchainImageCount);
    for (uint32_t i = 0; i < swapchainImageCount; ++i) {
        VkImageView attachments[] = {
            imageViews[i]
        };

        VkFramebufferCreateInfo framebufferCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = renderPass,
            .attachmentCount = 1,
            .pAttachments = attachments,
            .width = imageExtent.width,
            .height = imageExtent.height,
            .layers = 1
        };

        vkCreateFramebuffer(device, &framebufferCreateInfo, NULL, &framebuffers[i]);
    }
}

void createCommandBuffers() {
    VkCommandPoolCreateInfo poolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = graphicsQueueFamilyIndex
    };

    vkCreateCommandPool(device, &poolCreateInfo, NULL, &commandPool);

    commandBuffers = malloc(sizeof(VkCommandBuffer) * swapchainImageCount);
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = swapchainImageCount
    };

    vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffers);

    for (uint32_t i = 0; i < swapchainImageCount; ++i) {
        VkCommandBufferBeginInfo commandBufferBeginInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO
        };
        VkResult ress = vkBeginCommandBuffer(commandBuffers[i], &commandBufferBeginInfo);

        VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
        VkRenderPassBeginInfo renderPassBeginInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = renderPass,
            .framebuffer = framebuffers[i],
            .renderArea = {.offset = {0, 0}, .extent = imageExtent },
            .clearValueCount = 1,
            .pClearValues = &clearColor
        };

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

        VkViewport viewport = {
            .x = 0.0f,
            .y = 0.0f,
            .width = imageExtent.width,
            .height = imageExtent.height,
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        };
        vkCmdSetViewport(commandBuffers[i], 0, 1, &viewport);

        VkRect2D scissor = {
            .offset = { 0, 0 },
            .extent = imageExtent
        };
        vkCmdSetScissor(commandBuffers[i], 0, 1, &scissor);

        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, &vertexBuffer, offsets);
        vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, NULL);
        vkCmdDrawIndexed(commandBuffers[i], sizeof(indices) / sizeof(uint32_t), 1, 0, 0, 0);
        vkCmdEndRenderPass(commandBuffers[i]);
        VkResult res = vkEndCommandBuffer(commandBuffers[i]);
    }
}

void switchToFullscreen(HWND hwnd, int width, int height) {
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~WS_OVERLAPPEDWINDOW;
    style |= WS_POPUP;

    SetWindowLong(hwnd, GWL_STYLE, style);
    SetWindowPos(hwnd, HWND_TOP, 0, 0, width, height, SWP_FRAMECHANGED | SWP_NOOWNERZORDER);
    ShowWindow(hwnd, SW_SHOWMAXIMIZED);
}

void createSwapchain() {
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

    uint32_t surfaceFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, NULL);

    VkSurfaceFormatKHR* surfaceFormats = malloc(sizeof(VkSurfaceFormatKHR) * surfaceFormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, surfaceFormats);

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, NULL);

    VkPresentModeKHR* presentModes = malloc(sizeof(VkPresentModeKHR) * presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes);

    VkSurfaceFormatKHR surfaceFormat;
    for (uint32_t i = 0; i < surfaceFormatCount; ++i) {
        if (surfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB
            && surfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            surfaceFormat = surfaceFormats[i];
        }
    }

    free(surfaceFormats);
    swapChainFormat = surfaceFormat.format;

    VkPresentModeKHR presentMode;
    for (uint32_t i = 0; i < presentModeCount; ++i) {
        if (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
            presentMode = presentModes[i];
        }
    }
    free(presentModes);

    if (capabilities.currentExtent.width == 0xFFFFFFFF) {
        imageExtent.width = width;
        imageExtent.height = height;

        if (imageExtent.width < capabilities.minImageExtent.width) {
            imageExtent.width = capabilities.minImageExtent.width;
        }
        else if (imageExtent.width > capabilities.maxImageExtent.width) {
            imageExtent.width = capabilities.maxImageExtent.width;
        }

        if (imageExtent.height < capabilities.minImageExtent.height) {
            imageExtent.height = capabilities.minImageExtent.height;
        }
        else if (imageExtent.height > capabilities.maxImageExtent.height) {
            imageExtent.height = capabilities.maxImageExtent.height;
        }
    }
    else {
        imageExtent = capabilities.currentExtent;
    }

    VkSurfaceFullScreenExclusiveWin32InfoEXT fullscreenExclusiveInfo = {
        .sType = VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_WIN32_INFO_EXT,
        .pNext = NULL,
        .hmonitor = MonitorFromWindow(windowHandle, MONITOR_DEFAULTTONEAREST) };

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
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = imageExtent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = swapchain,
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

    VkSwapchainKHR newSwapchain;
    VkResult res = vkCreateSwapchainKHR(device, &swapchainCreateInfo, NULL, &newSwapchain);

    bool recreate = false;
    if (swapchain != VK_NULL_HANDLE)
    {
        vkDeviceWaitIdle(device);
        destroyCommandBuffers();
        destroyFramebuffers();
        destroyImageViews();
        destroySwapchain();
        SetWindowPos(windowHandle, NULL, 100, 100, 1000, 800, SWP_FRAMECHANGED | SWP_NOOWNERZORDER);
        recreate = true;
    }
    swapchain = newSwapchain;

    vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, NULL);
    swapchainImages = malloc(sizeof(VkImage) * swapchainImageCount);
    vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImages);

    if (recreate) {
        createImageViews();
        createFramebuffers();
        createCommandBuffers();
        
        switchToFullscreen(windowHandle, 1920, 1080);
        VkResult rrr = vkAcquireFullScreenExclusiveModeEXT2(device, swapchain);
        VkResult kkk = rrr;
    }
}

void createSynchObjects() {
    VkSemaphoreCreateInfo semaphoreInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
    };

    VkFenceCreateInfo fenceInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        vkCreateSemaphore(device, &semaphoreInfo, NULL, &imageAvailableSemaphores[i]);
        vkCreateSemaphore(device, &semaphoreInfo, NULL, &renderFinishedSemaphores[i]);
        vkCreateFence(device, &fenceInfo, NULL, &inFlightFences[i]);
    }
}

void destroySynchObjects() {
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        vkDestroyFence(device, inFlightFences[i], NULL);
        vkDestroySemaphore(device, imageAvailableSemaphores[i], NULL);
        vkDestroySemaphore(device, renderFinishedSemaphores[i], NULL);
    }
}

void initialize(HINSTANCE hInstance, HWND hWnd) {
    windowHandle = hWnd;

    camera_init_quaternion(
        &camera,
        (Quaternion) { .w = 1.0f, .x = 0.0f, .y = 0.0f, .z = 0.0f },
        (Vector3) { .x = 0, .y = 0, .z = 0 });

    createInstance();
    createSurface(hInstance, hWnd);
    getPhysicalDevice();
    getQueueFamilies();
    createLogicalDevice();
    createSwapchain();
    createImageViews();
    createRenderPass();
    createDescriptorSetLayout();
    createPipeline();
    createFramebuffers();
    createUniformBuffers();

    createTextureImage();
    createTextureImageView();
    createTextureSampler();

    createDescriptorPool();
    createDescriptorSet();
    createVertexBuffer();
    createIndexBuffer();
    createSynchObjects();
    createCommandBuffers();
}

void render() {
    vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    updateUniformBuffer(currentFrame);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        createSwapchain();
        return;
    }

    vkResetFences(device, 1, &inFlightFences[currentFrame]);

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
    VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffers[imageIndex],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signalSemaphores
    };

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]);

    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = signalSemaphores,
        .swapchainCount = 1,
        .pSwapchains = &swapchain,
        .pImageIndices = &imageIndex
    };

    vkQueuePresentKHR(presentQueue, &presentInfo);
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void deinitialize() {
    vkDeviceWaitIdle(device);
    destroySynchObjects();

    vkDestroyPipeline(device, pipeline, NULL);
    vkDestroyPipelineLayout(device, pipelineLayout, NULL);
    vkDestroyPipelineCache(device, pipelineCache, NULL);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(device, uniformBuffers[i], NULL);
        vkFreeMemory(device, uniformBuffersMemory[i], NULL);
    }

    vkDestroyBuffer(device, vertexBuffer, NULL);
    vkFreeMemory(device, vertexBufferMemory, NULL);

    vkDestroyBuffer(device, indexBuffer, NULL);
    vkFreeMemory(device, indexBufferMemory, NULL);

    destroyFramebuffers();
    destroyCommandBuffers();
    destroyImageViews();
    destroyRenderPass();
    destroySwapchain();
    destroySurface();

    vkDestroyDevice(device, NULL);
    vkDestroyInstance(instance, NULL);
}

void moveForward() {
    camera_move(&camera, 0.1f);
}

void moveLeft() {
    camera_yaw(&camera, radians(-0.1f));
}

void moveRight() {
    camera_pitch(&camera, radians(10));
}

void moveBackward() {
    camera_move(&camera, -0.1f);
}


Quaternion correct_roll(Quaternion current_rotation) {
    return quaternion_multiply(
        current_rotation,
        quaternion_angle_axis(
            -quaternion_extract_roll_angle(current_rotation),
            vector3_normalize(/*quaternion_rotate_vector(current_rotation, */vector3_unit_z/*)*/)));
}

/*
Quaternion quaternion_from_forward_up(Vector3 forward, Vector3 up) {
    Vector3 right = vector3_normalize(vector3_cross(up, forward));
    up = vector3_normalize(vector3_cross(forward, right));

    Quaternion q;
    q.w = sqrtf(1.0f + right.x + up.y + forward.z) * 0.5f;
    float w4 = (4.0f * q.w);
    q.x = (up.z - forward.y) / w4;
    q.y = (forward.x - right.z) / w4;
    q.z = (right.y - up.x) / w4;

    return q;
}

Quaternion correct_roll(Quaternion current_rotation) {
    Vector3 forward = quaternion_rotate_vector(current_rotation, (Vector3) { 0.0f, 0.0f, 1.0f });
    Vector3 world_up = { 0.0f, 1.0f, 0.0f };
    Vector3 right = vector3_normalize(vector3_cross(world_up, forward));
    Vector3 corrected_up = vector3_normalize(vector3_cross(forward, right));
    Quaternion corrected_rotation = quaternion_from_forward_up(forward, corrected_up);

    return corrected_rotation;
}*/

//
/*
Quaternion correct_roll(Quaternion current_rotation) {
    // Extract forward and right vectors from the current rotation
    Vector3 forward = quaternion_rotate_vector(current_rotation, (Vector3) { 0.0f, 0.0f, 1.0f });
    Vector3 right = quaternion_rotate_vector(current_rotation, (Vector3) { 1.0f, 0.0f, 0.0f });

    // Calculate the world up vector
    Vector3 world_up = { 0.0f, 1.0f, 0.0f };
    Vector3 actual_up = vector3_cross(forward, right);
    actual_up = vector3_normalize(actual_up);

    // Calculate the deviation between actual up and world up
    Vector3 cross = vector3_cross(actual_up, world_up);
    float dot = actual_up.x * world_up.x + actual_up.y * world_up.y + actual_up.z * world_up.z;
    float angle = acosf(dot);

    // Check if the angle is significant to correct
    if (fabsf(angle) < 0.0001f) {
        return current_rotation;
    }

    // Normalize the correction axis
    Vector3 correction_axis = vector3_normalize(cross);
    Quaternion correction = quaternion_angle_axis(angle * 180.0f / M_PI, correction_axis);
    Quaternion corrected_rotation = quaternion_multiply(correction, current_rotation);

    return corrected_rotation;
}*/



void mouseMove(float x, float y) {
    camera_pitch(&camera, radians(y * -0.1f));
    camera_yaw(&camera, radians(x * 0.1f));
}

void enableFullScreen() {
    width = 1920;
    height = 1080;

    ShowCursor(false);
    createSwapchain();
}