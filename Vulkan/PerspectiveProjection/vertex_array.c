#include "vertex_array.h"
#include "mesh_data.h"
#include <stdlib.h>
#include <stdio.h>

void init_vertex_array(VertexArray* array, size_t capacity) {
    array->size = 0;
    array->capacity = capacity;
    array->vertices = malloc(array->capacity * sizeof(int));
}

void realloc_vertex_array(VertexArray* array) {
    array->capacity *= 2;
    array->vertices = realloc(array->vertices, array->capacity * sizeof(Vertex));
}

void add_vertex(VertexArray* array, const char* const line) {
    if (array->size == array->capacity) {
        reallocate(array);
    }

    sscanf(line,
        "v %f %f %f",
        &array->vertices[array->size].x,
        &array->vertices[array->size].y,
        &array->vertices[array->size].z
    );

    ++array->size;
}

void free_vertex_array(VertexArray* array) {
    free(array->vertices);
    array->vertices = NULL;
    array->size = 0;
    array->capacity = 0;
}
