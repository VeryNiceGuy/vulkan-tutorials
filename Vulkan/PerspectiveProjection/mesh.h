#pragma once
#include "buffer.h"

/*
typedef struct RgbColor {
    float r, g, b;
} RgbColor;

typedef struct MaterialProp {
    RgbColor ambient;
    RgbColor diffuse;
    RgbColor specular;
    RgbColor emissive;
    float glossiness;
    float opacity;
    float transparency;
    uint32_t illumination_model;
    bool hasDiffuseMap;
    bool hasNormalMap;
    bool hasEmissiveMap;
    bool hasAlphaMap;
    bool hasBumpMap;
    bool hasDisplacementMap;
} MaterialProp;*/

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

