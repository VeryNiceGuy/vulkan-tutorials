#include "dynamic_array.h"
#include <stdlib.h>

void init_dynamic_array(
    DynamicArray* array,
    size_t element_size,
    size_t capacity) {

    array->size = 0;
    array->capacity = capacity;
    array->element_size = element_size;
    array->elements = malloc(array->capacity * array->element_size);
}

void realloc_dynamic_array(DynamicArray* array) {
    array->capacity *= 2;
    array->elements = realloc(array->elements, array->capacity * array->element_size);
}

void* get_element_ptr(DynamicArray* array, size_t index) {
    return &((unsigned char*)array->elements)[index * array->element_size];
}

void* get_next_ptr(DynamicArray* array) {
    if (array->size == array->capacity) {
        realloc_dynamic_array(array);
    }
    return get_element_ptr(array, array->size++);
}

void free_dynamic_array(DynamicArray* array) {
    free(array->elements);
    array->elements = NULL;
    array->size = 0;
    array->capacity = 0;
    array->element_size = NULL;
}
