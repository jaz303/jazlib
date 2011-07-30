#include "jazlib/common.h"

#include <stdlib.h>
#include <string.h>

/* hash functions from http://www.cse.yorku.ca/~oz/hash.html */

gh_hash_t hash_djb2(const char* key) {
    const unsigned char *str = (const unsigned char *)key;
    gh_hash_t hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}
  
gh_hash_t hash_sdbm(const char* key) {
    const unsigned char *str = (const unsigned char *)key;
    gh_hash_t hash = 0;
    int c;
    while ((c = *str++))
        hash = c + (hash << 6) + (hash << 16) - hash;
    return hash;
}

int gen_strcpy(const char *v, const char **t) {
    if (!v) *t = NULL;
    else {
        char *out = malloc(strlen(v) + 1);
        if (!*out) return 0;
        strcpy(out, v);
        *t = out;
    }
    return 1;
}
