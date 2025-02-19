#define _USE_MATH_DEFINES
#include <math.h>

#include "perspective_projection.h"
#include "vector3.h"
#include "matrix4x4.h"
#include "camera.h"
#include "tga.h"
#include "memory.h"
#include "buffer.h"
#include "swapchain.h"
#include "common.h"
#include "shader.h"
#include "pipeline.h"
#include "texture.h"
#include "render_pass.h"

HWND windowHandle;
VkInstance instance;
VkDevice device;
VkPhysicalDevice physicalDevice;
VkSurfaceKHR surface;

Pipeline pipeline;
Swapchain swapchain;

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

        VkPhysicalDeviceImageFormatInfo2 formatInfo = {0};
        formatInfo.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2;
        formatInfo.format = format;
        formatInfo.type = VK_IMAGE_TYPE_2D;
        formatInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        formatInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
        formatInfo.flags = 0;

        VkImageFormatProperties2 imageFormatProperties = {0};
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

void destroyFramebuffers() {
    for (uint32_t i = 0; i < swapchain.swapchainImageCount; ++i) {
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

        vkUpdateDescriptorSets(device, 2, descriptorWrites, 0, NULL);
    }
}

void createUniformBuffers() {
    mvp = (struct MVP){
        .model = matrix4x4_create_identity(),
        .view = (Matrix4x4){0},
        .proj = matrix4x4_perspective_fov_lh_gl(0.785398f, 800.0f / 600.0f, 0.1f, 100.0f)
    };

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        createUniformBuffer(physicalDevice, device, &uniformBuffers[i], &uniformBuffersMemory[i], sizeof(mvp));
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

void destroyRenderPass() {
    vkDestroyRenderPass(device, renderPass, NULL);
    renderPass = VK_NULL_HANDLE;
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
    framebuffers = malloc(sizeof(VkFramebuffer) * swapchain.swapchainImageCount);
    for (uint32_t i = 0; i < swapchain.swapchainImageCount; ++i) {
        VkImageView attachments[] = {
            swapchain.swapchainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = renderPass,
            .attachmentCount = 1,
            .pAttachments = attachments,
            .width = swapchain.imageExtent.width,
            .height = swapchain.imageExtent.height,
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

    commandBuffers = malloc(sizeof(VkCommandBuffer) * swapchain.swapchainImageCount);
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = swapchain.swapchainImageCount
    };

    vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffers);

    for (uint32_t i = 0; i < swapchain.swapchainImageCount; ++i) {
        VkCommandBufferBeginInfo commandBufferBeginInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO
        };
        VkResult ress = vkBeginCommandBuffer(commandBuffers[i], &commandBufferBeginInfo);

        VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
        VkRenderPassBeginInfo renderPassBeginInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = renderPass,
            .framebuffer = framebuffers[i],
            .renderArea = {.offset = {0, 0}, .extent = swapchain.imageExtent },
            .clearValueCount = 1,
            .pClearValues = &clearColor
        };

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline);

        VkViewport viewport = {
            .x = 0.0f,
            .y = 0.0f,
            .width = swapchain.imageExtent.width,
            .height = swapchain.imageExtent.height,
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        };
        vkCmdSetViewport(commandBuffers[i], 0, 1, &viewport);

        VkRect2D scissor = {
            .offset = { 0, 0 },
            .extent = swapchain.imageExtent
        };
        vkCmdSetScissor(commandBuffers[i], 0, 1, &scissor);

        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, &vertexBuffer, offsets);
        vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.layout, 0, 1, &descriptorSets[currentFrame], 0, NULL);
        vkCmdDrawIndexed(commandBuffers[i], sizeof(indices) / sizeof(uint32_t), 1, 0, 0, 0);
        vkCmdEndRenderPass(commandBuffers[i]);
        VkResult res = vkEndCommandBuffer(commandBuffers[i]);
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
    createSwapchain2(
        physicalDevice,
        device,
        hWnd,
        surface,
        width,
        height,
        graphicsQueueFamilyIndex,
        presentQueueFamilyIndex,
        &swapchain);

    createRenderPass(device, swapchain.surfaceFormat.format, &renderPass);
    createDescriptorSetLayout();
    createPipeline(device, 2, descriptorSetLayouts, renderPass, &pipeline);
    createFramebuffers();
    createUniformBuffers();

    createTextureImageFromFile("image.tga", device, physicalDevice, &textureImage, &textureImageMemory);
    createTextureImageView(device, textureImage, &textureImageView);
    createTextureSampler(device, &textureSampler);

    createDescriptorPool();
    createDescriptorSet();
    createVertexBuffer(physicalDevice, device, &vertexBuffer, &vertexBufferMemory, sizeof(vertices), vertices);
    createIndexBuffer(physicalDevice, device, &indexBuffer, &indexBufferMemory, sizeof(indices), indices);
    createSynchObjects();
    createCommandBuffers();
}

void render() {
    vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    updateUniformBuffer(currentFrame);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device, swapchain.swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        //createSwapchain();
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
        .pSwapchains = &swapchain.swapchain,
        .pImageIndices = &imageIndex
    };

    vkQueuePresentKHR(presentQueue, &presentInfo);
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void deinitialize() {
    vkDeviceWaitIdle(device);
    destroySynchObjects();

    vkDestroyPipeline(device, pipeline.pipeline, NULL);
    vkDestroyPipelineLayout(device, pipeline.layout, NULL);
    vkDestroyPipelineCache(device, pipeline.cache, NULL);

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
    destroyRenderPass();
    destroySwapchain2(&swapchain);
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

void mouseMove(float x, float y) {
    camera_pitch(&camera, radians(y * -0.1f));
    camera_yaw(&camera, radians(x * 0.1f));
}

void enableFullScreen() {
    width = 1920;
    height = 1080;

    ShowCursor(false);
    vkAcquireFullScreenExclusiveModeEXT2(device, swapchain.swapchain);
    //createSwapchain();
}
