#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "dynamic_array.h"
#include "hashtable.h"

typedef struct ObjVertex {
    float x, y, z;
} ObjVertex;

typedef struct ObjTexCoord {
    float u, v;
} ObjTexCoord;

typedef struct ObjNormal {
    float x, y, z;
} ObjNormal;

typedef struct ObjRgbColor {
    float r, g, b;
} ObjRgbColor;

typedef struct ObjMaterial {
    char name[128];
    ObjRgbColor ambient;
    ObjRgbColor diffuse;
    ObjRgbColor specular;
    ObjRgbColor emissive;
    float glossiness;
    float opacity;
    float transparency;
    int illumination_model;
    char ambient_map_filepath[256];
    char diffuse_map_filepath[256];
    char specular_map_filepath[256];
    char emissive_map_filepath[256];
    char alpha_map_filepath[256];
    char bump_map_filepath[256];
    char displacement_map_filepath[256];
} ObjMaterial;

typedef struct ObjFace {
    int32_t vertex_indices[3];
    bool has_texcoords;
    int32_t texcoord_indices[3];
    bool has_normals;
    int32_t normal_indices[3];
} ObjFace;

typedef struct ObjMaterialFaceGroup {
    bool has_material;
    char material_name[128];
    DynamicArray faces;
} ObjMaterialFaceGroup;

typedef struct ObjObject {
    char name[128];
    DynamicArray material_face_groups;
    Hashtable material_face_group_map;
    DynamicArray groups;
} ObjObject;

typedef struct ObjGroup {
    char name[128];
    DynamicArray material_face_groups;
    Hashtable material_face_group_map;
} ObjGroup;

typedef struct ObjModel {
    DynamicArray vertices;
    DynamicArray texcoords;
    DynamicArray normals;
    DynamicArray material_face_groups;
    Hashtable material_face_group_map;
    DynamicArray objects;
    DynamicArray groups;
    DynamicArray materials;
    Hashtable material_map;
} ObjModel;

void load_obj_model(const char* file_path, struct ObjModel* model);
