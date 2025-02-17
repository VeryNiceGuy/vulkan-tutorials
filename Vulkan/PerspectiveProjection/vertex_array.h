#pragma once
#include <stddef.h>

struct Vertex;
typedef struct Vertex Vertex;

typedef struct {
    size_t size;
    size_t capacity;
    Vertex* vertices;
} VertexArray;

void init_vertex_array(VertexArray* array, size_t capacity);
void realloc_vertex_array(VertexArray* array);
void add_vertex(VertexArray* array, const char* const line);
void free_vertex_array(VertexArray* array);
