#include "normal_array.h"
#include "mesh_data.h"
#include <stdlib.h>

void init_normal_array(NormalArray* array, size_t capacity) {
    array->size = 0;
    array->capacity = capacity;
    array->normals = malloc(array->capacity * sizeof(Normal));
}

void realloc_normal_array(NormalArray* array) {
    array->capacity *= 2;
    array->normals = realloc(array->normals, array->capacity * sizeof(Normal));
}

void add_normal(NormalArray* array, const char* const line) {
    if (array->size == array->capacity) {
        reallocate(array);
    }

    sscanf(
        line,
        "vn %f %f %f",
        &array->normals[array->size].x,
        &array->normals[array->size].y,
        &array->normals[array->size].z
    );

    ++array->size;
}

void free_normal_array(NormalArray* array) {
    free(array->normals);
    array->normals = NULL;
    array->size = 0;
    array->capacity = 0;
}
