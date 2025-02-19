#include "shader.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

VkShaderModule createShaderModule(VkDevice device, const char* filename) {
    FILE* file = fopen(filename, "rb");
    fseek(file, 0, SEEK_END);
    size_t codeSize = ftell(file);
    rewind(file);

    uint8_t* code = malloc(codeSize);
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