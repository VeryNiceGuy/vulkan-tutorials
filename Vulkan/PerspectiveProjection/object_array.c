#include "object_array.h"
#include "obj_data.h"
#include <stdlib.h>

void init_object_array(ObjectArray* array, size_t capacity) {
    array->size = 0;
    array->capacity = capacity;
    array->objects = malloc(array->capacity * sizeof(Object));
}

void realloc_object_array(ObjectArray* array) {
    array->capacity *= 2;
    array->objects = realloc(array->objects, array->capacity * sizeof(Object));
}

Object* add_object(ObjectArray* array, const char* const line) {
    if (array->size == array->capacity) {
        reallocate(array);
    }

    sscanf(line, "o %127s", &array->objects[array->size].name);
    return &array->objects[array->size++];
}

void free_object_array(ObjectArray* array) {
    free(array->objects);
    array->objects = NULL;
    array->size = 0;
    array->capacity = 0;
}
