#include <assert.h>
#include <stdio.h>

#include "kv.h"

kv_entry_t *table = NULL;

int main(int argc, char **argv)
{
    uint8_t *buf = malloc(16);
    memset(buf, 0x53, 15); memset(buf + 15, '\n', 1);
    assert(kv_put(&table, "foo", buf, 16, true));

    kv_log_table(table);

    uint8_t *val;
    uint32_t val_len;
    assert(!kv_get(table, "bar", &val, &val_len));
    printf("val: %p\n", val);
    assert(kv_get(table, "foo", &val, &val_len));
    printf("val: %p\n", val);

    printf("val_len: %u\n", val_len);

    for (uint32_t i = 0; i < val_len; i++) {
        printf("%c", val[i]);
    }
    printf("\n");

    assert(kv_write_table(table));

    assert(kv_del(&table, "foo"));

    kv_log_table(table);
    printf("---\n");

    assert(kv_put(&table, "1", (uint8_t *)"A", 1, false));
    assert(kv_put(&table, "2", (uint8_t *)"B", 1, false));
    assert(kv_put(&table, "3", (uint8_t *)"C", 1, false));
    assert(kv_put(&table, "4", (uint8_t *)"D", 1, false));

    kv_log_table(table);

    assert(kv_write_table(table));
    assert(kv_destroy(&table));

    kv_log_table(table);

    printf("---\n");
    assert(kv_destroy(&table));
    kv_log_table(table);

    assert(kv_read_table(&table));

    kv_log_table(table);

    return 0;
}
