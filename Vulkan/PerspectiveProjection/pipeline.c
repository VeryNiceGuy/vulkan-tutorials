#include "pipeline.h"
#include "common.h"
#include "shader.h"

void createPipeline(
    VkDevice device,
    uint32_t descriptorSetLayoutCount,
    VkDescriptorSetLayout* descriptorSetLayouts,
    VkRenderPass renderPass,
    Pipeline* pipeline) {

    VkPipelineShaderStageCreateInfo shaderStageCreateInfos[2] = { {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = createShaderModule(device, "vertex_shader.spv"),
            .pName = "main"
        }, {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = createShaderModule(device, "fragment_shader.spv"),
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
        .pVertexAttributeDescriptions = vertexInputAttributeDescriptions
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

    VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pDynamicStates = dynamicStates,
        .dynamicStateCount = 2
    };

    VkPipelineLayoutCreateInfo layoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = descriptorSetLayoutCount,
        .pSetLayouts = descriptorSetLayouts
    };

    vkCreatePipelineLayout(device, &layoutCreateInfo, NULL, &pipeline->layout);

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = shaderStageCreateInfos,
        .pVertexInputState = &vertexInputStateCreateInfo,
        .pInputAssemblyState = &inputAssemblyStateCreateInfo,
        .pRasterizationState = &rasterizationStateCreateInfo,
        .pMultisampleState = &multisampleStateCreateInfo,
        .pColorBlendState = &colorBlendStateCreateInfo,
        .pDynamicState = &dynamicStateCreateInfo,
        .renderPass = renderPass,
        .layout = pipeline->layout
    };

    VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO
    };

    vkCreatePipelineCache(device, &pipelineCacheCreateInfo, NULL, &pipeline->cache);
    vkCreateGraphicsPipelines(device, pipeline->cache, 1, &pipelineCreateInfo, NULL, &pipeline->pipeline);
}
