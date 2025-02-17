#pragma once
#include <stdbool.h>

typedef struct Vertex {
    float x, y, z;
} Vertex;

typedef struct TexCoord {
    float u, v;
} TexCoord;

typedef struct Normal {
    float x, y, z;
} Normal;

typedef struct Material {
    char name[128];
    float Ka[3];  // Ambient color
    float Kd[3];  // Diffuse color
    float Ks[3];  // Specular color
    float Ke[3];  // Emissive color
    float Ns;     // Specular exponent
    float Ni;     // Optical density
    float d;      // Dissolve (transparency)
    int illum;    // Illumination model
    char map_Ka[256];  // Ambient texture map
    char map_Kd[256];  // Diffuse texture map
    char map_Ks[256];  // Specular texture map
    char map_Ke[256];  // Emissive texture map
    char map_d[256];   // Alpha texture map
    char map_Bump[256];  // Bump map
    char map_disp[256];  // Displacement map
} Material;

typedef struct FaceMaterial {
    char name[256];
} FaceMaterial;

typedef struct Face {
    int material_index;
    int vertex_indices[3];

    bool has_texcoords;
    int texcoord_indices[3];

    bool has_normals;
    int normal_indices[3];
} Face;
