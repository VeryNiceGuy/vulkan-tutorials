#pragma once
#include <stddef.h>

struct Group;
typedef struct Group Group;

typedef struct {
    size_t size;
    size_t capacity;
    Group* groups;
} GroupArray;

void init_group_array(GroupArray* array, size_t capacity);
void realloc_group_array(GroupArray* array);
Group* add_group(GroupArray* array, const char* const line);
void free_group_array(GroupArray* array);
