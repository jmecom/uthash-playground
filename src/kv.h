#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "uthash.h"

#define MAX_KEY_LEN (16)

typedef struct kv_entry_t {
    char key[MAX_KEY_LEN];
    uint8_t *value;
    uint32_t value_len;
    bool should_free;
    UT_hash_handle hh; // Makes this structure hashable
} kv_entry_t;

// Place a (key, value) pair into the table.
bool kv_put(kv_entry_t **table, const char *key, uint8_t *value, uint32_t value_len, bool should_free);

// Retrieve a (key, value) pair from the table.
bool kv_get(kv_entry_t *table, const char *key, uint8_t **value, uint32_t *value_len);

// Delete a (key, value) pair from the table.
bool kv_del(kv_entry_t **table, const char *key);

// Delete all entries in the table, free the table, and set table to null.
bool kv_destroy(kv_entry_t **table);

// Read a table from storage (a file in this case).
bool kv_read_table(kv_entry_t **table);

// Write a table from storage (a file in this case).
bool kv_write_table(kv_entry_t *table);

// Print all entries in the table.
void kv_log_table(kv_entry_t *table);
