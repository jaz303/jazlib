#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#define MIN(a,b) ((a)<(b)?(a):(b))

#include "jazlib/common.h"

#include "jazlib/gen_hash_reset.h"
#define GH_DEBUG
#define GEN_HASH_HASH_FUNC      hash_djb2
#define GEN_HASH_KEY_CMP        strcmp
// #define GEN_HASH_KEY_COPY
// #define GEN_HASH_KEY_FREE
#define GEN_HASH_VALUE_CMP      strcmp
// #define GEN_HASH_VALUE_COPY
// #define GEN_HASH_VALUE_FREE
#include "jazlib/gen_hash.h"
GEN_HASH(hash, const char *, const char *);

typedef struct hash_test {
    char    key[16];
    char    value[16];
    int     in_table;
} hash_test_t;

#define COUNT   480000
#define PASSES  200

hash_test_t table[COUNT];

void random_string(char *target, int suffix) {
    int len = (rand() % 8) + 1;
    while (len--) *(target++) = (rand() % 26) + 'a';
    sprintf(target, "%d", suffix);
}

void check(hash_t *hsh) {
    unsigned long sz = 0;
    int i;
    for (i = 0; i < COUNT; i++) {
        if (table[i].in_table) sz++;
        const char *value;
        int found = hash_read(hsh, table[i].key, &value);
        if (found != table[i].in_table) {
            printf("error (in_table=%d, ix=%d k=%s)\n", table[i].in_table, i, table[i].key);
        } else if (found && (strcmp(value, table[i].value) != 0)) {
            printf("cmp error\n");
        }
    }
    if (sz != hsh->size) {
        printf("size error (exp=%u,rep=%u)\n", (int)sz, hsh->size);
    }
}

int main(int argc, char *argv[]) {
    
    srand(time(NULL));
    
    int i;
    for (i = 0; i < COUNT; i++) {
        random_string(table[i].key, i);
        random_string(table[i].value, i);
        table[i].in_table = 0;
        if (((i+1)%50000) == 0) {
            printf("%d random pairs generated\n", i+1);
        }
    }
    
    hash_t hsh;
    hash_init(&hsh);

    unsigned long ops = 0;

    int j;
    for (j = 0; j < PASSES; j++) {
        int ins = (j % 4 == 0) ? 0 : 1;
        int range = (rand() % 30000) + 1;
        ops += range;
        int min = rand() % COUNT;
        int max = MIN(COUNT, min + range);
        printf("Pass %d/%d %s (%d) %d-%d\n", j+1, PASSES, ins ? "insert" : "delete", range, min, max);
        GH_DEBUG_PRINT(&hsh);
        if (ins) {
            for (i = min; i < max; i++) {
                gh_hash_t sz = hsh.size;
                if (hash_put(&hsh, table[i].key, table[i].value)) {
                    if (!table[i].in_table) {
                        assert(sz + 1 == hsh.size);
                    } else if (sz != hsh.size) {
                        printf("expected=%u actual=%u\n", sz, hsh.size);
                        assert(0);
                    }
                    table[i].in_table = 1;
                } else {
                    printf("put error\n");
                    exit(1);
                }
            }
        } else {
            for (i = min; i < max; i++) {
                gh_hash_t sz = hsh.size;
                if (hash_delete(&hsh, table[i].key)) {
                    assert(sz - 1 == hsh.size);
                    table[i].in_table = 0;
                } else if (table[i].in_table) {
                    printf("delete error\n");
                    exit(1);
                }
            }
        }
        check(&hsh);
    }
    
    printf("ops: %lu\n", ops);
    
    return 0;

}

