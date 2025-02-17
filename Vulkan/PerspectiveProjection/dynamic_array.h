#pragma once
#include <stddef.h>

typedef struct DynamicArray {
    size_t size;
    size_t capacity;
    size_t element_size;
    void* elements;
} DynamicArray;

void init_dynamic_array(
    DynamicArray* array,
    size_t element_size,
    size_t capacity
);
void realloc_dynamic_array(DynamicArray* array);
void* get_element_ptr(DynamicArray* array, size_t index);
void* get_next_ptr(DynamicArray* array);
void free_dynamic_array(DynamicArray* array);
