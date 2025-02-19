#pragma once
#include "buffer.h"

typedef struct Material {
    VkDescriptorImageInfo* diffuseMap;
    VkDescriptorImageInfo* normalMap;
    VkDescriptorImageInfo* specularMap;
    VkDescriptorImageInfo* emissiveMap;
    VkDescriptorImageInfo* alphaMap;
    VkDescriptorImageInfo* bumpMap;
    VkDescriptorImageInfo* displacementMap;
} Material;

typedef struct SubMesh {
	Material* material;
} SubMesh;

typedef struct Mesh {
	Buffer vertexBuffer;
	Buffer indexBuffer;
	size_t subMeshCount;
	SubMesh* subMeshes;
} Mesh;

