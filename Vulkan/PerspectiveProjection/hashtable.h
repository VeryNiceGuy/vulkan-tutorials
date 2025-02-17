#pragma once
#include <stdbool.h>
#include <stddef.h>

typedef struct Entry {
    char* key;
    size_t value;
    struct Entry* next;
} Entry;

typedef struct Hashtable {
    Entry** head;
    size_t table_size;
    size_t entry_count;
} Hashtable;

void init_hashtable(Hashtable* hashtable, size_t size);
void free_hashtable(Hashtable* hashtable);
void insert_hashtable(Hashtable* hashtable, char* key, size_t value);
bool search_hashtable(Hashtable* hashtable, char* key, size_t* value);
void delete_hashtable(Hashtable* hashtable, char* key);