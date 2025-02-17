#pragma once
#include <stddef.h>

struct Face;
typedef struct Face Face;

typedef struct {
    size_t size;
    size_t capacity;
    Face* faces;
} FaceArray;

void init_face_array(FaceArray* array, size_t capacity);
void realloc_face_array(FaceArray* array);
void add_face(FaceArray* array, const char* const line);
void free_face_array(FaceArray* array);
