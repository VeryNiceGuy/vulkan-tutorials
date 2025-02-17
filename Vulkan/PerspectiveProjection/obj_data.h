#pragma once
#include "dynamic_array.h"

typedef struct Object {
    char name[128];
    DynamicArray faces;
    DynamicArray groups;
} Object;

typedef struct Group {
    char name[128];
    DynamicArray faces;
} Group;

typedef struct ObjModel {
    DynamicArray vertices;
    DynamicArray texcoords;
    DynamicArray normals;
    DynamicArray faces;
    DynamicArray objects;
    DynamicArray groups;
    DynamicArray materials;
} ObjModel;