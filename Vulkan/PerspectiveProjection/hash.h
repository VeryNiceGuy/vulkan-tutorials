#pragma once
#include <stddef.h>

unsigned int hash_djb2(char* key, size_t table_size);
unsigned int hash_sdbm(char* key, size_t table_size);
unsigned int hash_fnv_1a(char* key, size_t table_size);
