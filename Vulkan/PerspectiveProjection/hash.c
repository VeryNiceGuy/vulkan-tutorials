#include "hash.h"

unsigned int hash_djb2(char* key, size_t table_size) {
    unsigned int hash = 5381;
    while (*key) {
        hash = ((hash << 5) + hash) + *key++;
    }
    return hash % table_size;
}

unsigned int hash_sdbm(char* key, size_t table_size) {
    unsigned int hash = 0;
    while (*key) {
        hash = *key++ + (hash << 6) + (hash << 16) - hash;
    }
    return hash % table_size;
}

unsigned int hash_fnv_1a(char* key, size_t table_size) {
    unsigned int hash = 2166136261u;
    while (*key) {
        hash ^= (unsigned char)*key++;
        hash *= 16777619;
    }
    return hash % table_size;
}
