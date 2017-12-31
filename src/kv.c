#include "kv.h"

#include <stdio.h>

#define TABLE_PATH "/tmp/table"

const uint16_t magic = 0x1234;

bool kv_put(kv_entry_t **table, const char *key, uint8_t *value, uint32_t value_len, bool should_free)
{
    if (!table || !key || !value) {
        return false;
    }

    if (strnlen(key, MAX_KEY_LEN + 1) > MAX_KEY_LEN) {
        return false;
    }

    kv_entry_t *e;
    // note: key doesn't refer to the argument, but rather the name in the struct.
    HASH_FIND_STR(*table, key, e);
    if (e) {
        // key already present, don't add the entry
        return false;
    }

    e = malloc(sizeof(kv_entry_t));
    if (!e) {
        return false;
    }

    strncpy(e->key, key, MAX_KEY_LEN);

    HASH_ADD_STR(*table, key, e);

    e->value = value;
    e->value_len = value_len;

    // informs kv_del and kv_destroy if they should free the entry's value
    e->should_free = should_free;

    return true;
}

bool kv_get(kv_entry_t *table, const char *key, uint8_t **value, uint32_t *value_len)
{
    if (!table || !key || !value || !value_len) {
        return false;
    }

    kv_entry_t *e;
    HASH_FIND_STR(table, key, e);
    if (!e) {
        return false;
    }
    *value = e->value;
    *value_len = e->value_len;
    return true;
}

bool kv_del(kv_entry_t **table, const char *key)
{
    if (!table || !key) {
        return false;
    }

    kv_entry_t *e;
    HASH_FIND_STR(*table, key, e);
    if (!e) {
        return false;
    }

    if (e->should_free) {
        free(e->value);
    }

    HASH_DEL(*table, e);
    free(e);
    return true;
}

bool kv_destroy(kv_entry_t **table)
{
    if (!table) {
        return false;
    }

    kv_entry_t *cur, *tmp;

    // HASH_ITER is a deletion-safe iteration macro
    HASH_ITER(hh, *table, cur, tmp) {
        HASH_DEL(*table, cur);
        if (cur->should_free) {
            free(cur->value);
        }
        free(cur);
    }

    return true;
}

bool kv_read_table(kv_entry_t **table)
{
    if (!table) {
        return false;
    }

    FILE *fp = fopen(TABLE_PATH, "r");
    if (!fp) {
        return false;
    }

    uint16_t magic_read;
    fread(&magic_read, sizeof(magic_read), 1, fp);
    if (magic != magic_read) {
        return false;
    }

    uint32_t count;
    fread(&count, sizeof(count), 1, fp);

    uint8_t key_len;
    uint32_t value_len;
    char key[MAX_KEY_LEN];
    for (uint32_t i = 0; i < count; i++) {
        // Retrieve the key
        fread(&key_len, sizeof(key_len), 1, fp);
        if (key_len > MAX_KEY_LEN) {
            printf("key len: %u\n", key_len);
            return false;
        }
        fread(key, key_len, 1, fp);

        // Retrieve the value, allocate memory for it
        fread(&value_len, sizeof(value_len), 1, fp);
        uint8_t *value = malloc(value_len);
        if (!value) {
            return false;
        }
        fread(value, value_len, 1, fp);

        // Create and add the table entry
        if (!kv_put(table, key, value, value_len, true)) {
            return false;
        }
    }

    fclose(fp);

    return true;
}

// Store the table as:
//      Magic value 2 bytes
//      Num entries 4 bytes
// (repeat the below num entries times)
//      Key-len     1 byte
//      Key         key-len bytes
//      Value-len   4 bytes
//      Value       value-len bytes
bool kv_write_table(kv_entry_t *table)
{
    if (!table) {
        return false;
    }

    FILE *fp = fopen(TABLE_PATH, "w");
    if (!fp) {
        return false;
    }

    uint32_t num_entries = HASH_COUNT(table);
    fwrite(&magic, 1, sizeof(magic), fp);
    fwrite(&num_entries, 1, sizeof(num_entries), fp);

    kv_entry_t *e;
    for (e = table; e != NULL; e = (kv_entry_t *)(e->hh.next)) {
        uint8_t key_len = strnlen(e->key, MAX_KEY_LEN);

        fwrite(&key_len, 1, sizeof(key_len), fp);
        fwrite(e->key, 1, key_len, fp);

        fwrite(&(e->value_len), 1, sizeof(e->value_len), fp);
        fwrite(e->value, 1, e->value_len, fp);
    }

    fclose(fp);

    return true;
}

void kv_log_table(kv_entry_t *table)
{
    if (!table) {
        printf("Empty table.\n");
        return;
    }

    kv_entry_t *e;
    for (e = table; e != NULL; e = (kv_entry_t *)(e->hh.next)) {
        printf("%s: %s\n", e->key, e->value);
    }
}
