#pragma once
#include <stddef.h>

struct TexCoord;
typedef struct TexCoord TexCoord;

typedef struct {
    size_t size;
    size_t capacity;
    TexCoord* texcoords;
} TexCoordArray;

void init_texcoord_array(TexCoordArray* array, size_t capacity);
void realloc_texcoord_array(TexCoordArray* array);
void add_texcoord(TexCoordArray* array, const char* const line);
void free_texcoord_array(TexCoordArray* array);
