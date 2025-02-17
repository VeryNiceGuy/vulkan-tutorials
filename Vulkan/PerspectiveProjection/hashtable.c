#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "hashtable.h"

#define LOAD_FACTOR 0.75

Entry* create_entry(char* key, size_t value) {
    Entry* new_entry = malloc(sizeof(Entry));
    new_entry->key = _strdup(key);
    new_entry->value = value;
    new_entry->next = NULL;

    return new_entry;
}

void init_hashtable(Hashtable* hashtable, size_t size) {
    hashtable->head = calloc(size, sizeof(Entry*));
    hashtable->table_size = size;
    hashtable->entry_count = 0;
}

void free_hashtable(Hashtable* hashtable) {
    for (size_t i = 0; i < hashtable->table_size; ++i) {
        Entry* temp = hashtable->head[i];
        while (temp != NULL) {
            Entry* to_free = temp;
            temp = temp->next;
            free(to_free->key);
            free(to_free);
        }
    }
    free(hashtable->head);
}

void insert_hashtable(Hashtable* hashtable, char* key, size_t value);

void resize_hashtable(Hashtable* hashtable) {
    unsigned int old_table_size = hashtable->table_size;
    Entry** old_table = hashtable->head;

    hashtable->table_size *= 2;
    hashtable->head = calloc(hashtable->table_size, sizeof(Entry*));

    for (unsigned int i = 0; i < old_table_size; ++i) {
        Entry* temp = old_table[i];

        while (temp != NULL) {
            insert_hashtable(hashtable, temp->key, temp->value);
            Entry* to_free = temp;
            temp = temp->next;
            free(to_free->key);
            free(to_free);
        }
    }

    free(old_table);
}

void insert_hashtable(Hashtable* hashtable, char* key, size_t value) {
    if ((float)(hashtable->entry_count) / hashtable->table_size >= LOAD_FACTOR) {
        resize_hashtable(hashtable);
    }

    unsigned int index = hash_djb2(key, hashtable->table_size);
    Entry* new_entry = create_entry(key, value);

    if (hashtable->head[index] == NULL) {
        hashtable->head[index] = new_entry;
    } else {
        Entry* temp = hashtable->head[index];
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_entry;
    }

    ++hashtable->entry_count;
}

bool search_hashtable(Hashtable* hashtable, char* key, size_t* value) {
    size_t index = hash_djb2(key, hashtable->table_size);
    Entry* temp = hashtable->head[index];

    while (temp != NULL) {
        if (strcmp(temp->key, key) == 0) {
            if (value) {
                *value = temp->value;
            }
            return true;
        }
        temp = temp->next;
    }

    return false;
}

void delete_hashtable(Hashtable* hashtable, char* key) {
    size_t index = hash_djb2(key, hashtable->table_size);
    Entry* temp = hashtable->head[index];
    Entry* prev = NULL;

    while (temp != NULL && strcmp(temp->key, key) != 0) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        return;
    }

    if (prev == NULL) {
        hashtable->head[index] = temp->next;
    } else {
        prev->next = temp->next;
    }

    free(temp->key);
    free(temp);
    --hashtable->entry_count;
}
