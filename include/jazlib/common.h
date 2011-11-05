#ifndef __JAZLIB__COMMON_H__
#define __JAZLIB__COMMON_H__

#include "jazlib/gen_hash_common.h"

gh_hash_t hash_djb2(const char* key);
gh_hash_t hash_sdbm(const char* key);

/*
 * String copy function compatible with gen_hash.h and gen_vector.h
 * Allocates space for, and copies, string v, and stores its pointer in *t
 * Returns 0 on failure, non-zero on success
 */
int gen_strcpy(void *context, const char *v, const char **t);

#endif