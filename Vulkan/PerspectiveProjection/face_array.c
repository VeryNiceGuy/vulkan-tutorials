#include "face_array.h"
#include "mesh_data.h"
#include <stdlib.h>

void init_face_array(FaceArray* array, size_t capacity) {
    array->size = 0;
    array->capacity = capacity;
    array->faces = malloc(array->capacity * sizeof(Face));
}

void realloc_face_array(FaceArray* array) {
    array->capacity *= 2;
    array->faces = realloc(array->faces, array->capacity * sizeof(Face));
}

void add_face(FaceArray* array, const char* const line) {
    if (array->size == array->capacity) {
        reallocate(array);
    }

    sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
        &array->faces[array->size].vertex_indices[0],
        &array->faces[array->size].texcoord_indices[0],
        &array->faces[array->size].normal_indices[0],

        &array->faces[array->size].vertex_indices[1],
        &array->faces[array->size].texcoord_indices[1],
        &array->faces[array->size].normal_indices[1],

        &array->faces[array->size].vertex_indices[2],
        &array->faces[array->size].texcoord_indices[2],
        &array->faces[array->size].normal_indices[2]
    );

    ++array->size;
}

void free_face_array(FaceArray* array) {
    free(array->faces);
    array->faces = NULL;
    array->size = 0;
    array->capacity = 0;
}
