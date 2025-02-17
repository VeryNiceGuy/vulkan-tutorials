#include "group_array.h"
#include "obj_data.h"
#include <stdlib.h>

void init_group_array(GroupArray* array, size_t capacity) {
    array->size = 0;
    array->capacity = capacity;
    array->groups = malloc(array->capacity * sizeof(Group));
}

void realloc_group_array(GroupArray* array) {
    array->capacity *= 2;
    array->groups = realloc(array->groups, array->capacity * sizeof(Group));
}

Group* add_group(GroupArray* array, const char* const line) {
    if (array->size == array->capacity) {
        reallocate(array);
    }

    sscanf(line, "g %127s", &array->groups[array->size].name);
    return &array->groups[array->size++];
}

void free_group_array(GroupArray* array) {
    free(array->groups);
    array->groups = NULL;
    array->size = 0;
    array->capacity = 0;
}
