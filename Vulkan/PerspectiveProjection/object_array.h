#pragma once
#include <stddef.h>

struct Object;
typedef struct Object Object;

typedef struct {
    size_t size;
    size_t capacity;
    Object* objects;
} ObjectArray;

void init_object_array(ObjectArray* array, size_t capacity);
void realloc_object_array(ObjectArray* array);
Object* add_object(ObjectArray* array, const char* const line);
void free_object_array(ObjectArray* array);
