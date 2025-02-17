#include "string_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

char* trim_start(char* str) {
    if (str == NULL || *str == '\0') {
        return str;
    }

    char* start = str;
    while (isspace((unsigned char)*start)) {
        start++;
    }

    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }

    return str;
}

char* trim_end(char* str) {
    if (str == NULL || *str == '\0') {
        return str;
    }

    char* end = str + strlen(str) - 1;
    while (end >= str && isspace((unsigned char)*end)) {
        end--;
    }

    *(end + 1) = '\0';
    return str;
}

char* trim(char* str) {
    trim_start(str);
    trim_end(str);

    return str;
}