#include "texcoord_array.h"
#include "mesh_data.h"
#include <stdlib.h>
#include <stdio.h>

void init_texcoord_array(TexCoordArray* array, size_t capacity) {
    array->size = 0;
    array->capacity = capacity;
    array->texcoords = malloc(array->capacity * sizeof(int));
}

void realloc_texcoord_array(TexCoordArray* array) {
    array->capacity *= 2;
    array->texcoords = realloc(array->texcoords, array->capacity * sizeof(TexCoord));
}

void add_texcoord(TexCoordArray* array, const char* const line) {
    if (array->size == array->capacity) {
        reallocate(array);
    }

    sscanf(line,
        "vt %f %f",
        &array->texcoords[array->size].u,
        &array->texcoords[array->size].v
    );

    ++array->size;
}

void free_texcoord_array(TexCoordArray* array) {
    free(array->texcoords);
    array->texcoords = NULL;
    array->size = 0;
    array->capacity = 0;
}
