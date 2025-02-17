#pragma once
#include <stddef.h>

struct Normal;
typedef struct Normal Normal;

typedef struct {
    size_t size;
    size_t capacity;
    Normal* normals;
} NormalArray;

void init_normal_array(NormalArray* array, size_t capacity);
void realloc_normal_array(NormalArray* array);
void add_normal(NormalArray* array, const char* const line);
void free_normal_array(NormalArray* array);
