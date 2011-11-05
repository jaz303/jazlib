/*
 * A super-configurable generic hash.
 * 
 * Thoroughly and unashamedly inspired by attractivechaos's klib/khash
 * (https://github.com/attractivechaos/klib)
 *
 * TODO: explore constraining values to be pointers only, and using the 2 LSBs
 * for storing bucket state, rather than auxiliary array.
 * Pros: saves space, more readable - and probably faster - than the current bitshift nightmare.
 * Cons: assumes pointers are 32-bit aligned, can't store non-pointer types as hash values.
 * 
 * my_hash.h:
 * #include "gen_hash.h"
 * GEN_HASH_DECLARE(typename, const char *, int);
 *
 * my_hash.c:
 * #include "gen_hash_reset.h"
 * #define GEN_HASH_KEY_CMP     strcmp
 * #define GEN_HASH_KEY_COPY    hash_strcpy
 * #define GEN_HASH_KEY_FREE    free
 * #include "gen_hash.h"
 * GEN_HASH_INIT(typename, const char *, int);
 */

#ifndef __JAZLIB__GEN_HASH_H__
#define __JAZLIB__GEN_HASH_H__
    #include <string.h>
    #include <assert.h>
    
    #include "jazlib/gen_hash_common.h"
    
    #define GH_BUCKET_EMPTY                     0
    #define GH_BUCKET_FULL                      1
    #define GH_BUCKET_DELETED                   2
    
    #define GH_BUCKET_STATE(flags, ix)          ((flags[ix>>2]>>((ix&3)<<1))&3)
    #define GH_SET_BUCKET_STATE(flags, ix, s)   (flags[ix>>2]=(flags[ix>>2]&(~(3<<((ix&3)<<1))))|(s<<((ix&3)<<1)))
    
    #define GH_DEBUG_PRINT(hsh)                 printf("b=%lu occ=%lu sz=%lu ub=%lu\n", (unsigned long)(hsh)->n_buckets, (unsigned long)(hsh)->n_occupied, (unsigned long)(hsh)->size, (unsigned long)(hsh)->upper_bound)
    
    #ifdef GH_DEBUG
        #define __gh_debug 1
    #else
        #define __gh_debug 0
    #endif
#endif

/*
 * Configuration
 * #define these before #include'ing gen_hash.h
 */
 
/* malloc function (for internal data structures only, not keys/values), default is malloc() */
#ifdef GEN_HASH_MALLOC
    #define __gh_malloc(sz) (GEN_HASH_MALLOC(sz))
#else
    #include <stdlib.h>
    #define __gh_malloc(sz) malloc(sz)
#endif

/* realloc function (for internal data structures only, not keys/values), default is realloc() */
#ifdef GEN_HASH_REALLOC
    #define __gh_realloc(ptr, sz) (GEN_HASH_REALLOC(ptr, sz))
#else
    #include <stdlib.h>
    #define __gh_realloc(ptr, sz) realloc(ptr, sz)
#endif

/* free function (for internal data structures only, not keys/values), default is free() */
#ifdef GEN_HASH_FREE
    #define __gh_free(ptr) (GEN_HASH_FREE(ptr))
#else
    #include <stdlib.h>
    #define __gh_free(ptr) free(ptr)
#endif
 
/* max load factor before resizing hash table */
#ifndef GEN_HASH_MAX_LOAD
    #define GEN_HASH_MAX_LOAD 0.7
#endif

#ifndef GEN_HASH_BUCKET_SIZES
    #define GEN_HASH_BUCKET_SIZES \
        0,          3,          11,         23,         53,         97, \
        193,        389,        769,        1543,       3079,       6151, \
        12289,      24593,      49157,      98317,      196613,     393241, \
        786433,     1572869,    3145739,    6291469,    12582917,   25165843, \
        50331653,   100663319,  201326611,  402653189,  805306457,  1610612741
#endif

/*
 * hash function, required.
 * takes a key as input, returns hash code.
 */
#ifdef GEN_HASH_HASH_FUNC
    #define __gh_hash_key(k) (GEN_HASH_HASH_FUNC(k))
#else
    #define __gh_hash_key(k) ((gh_hash_t)k)
#endif

/*
 * function used to compare keys.
 * should return 0 on equality, non-zero otherwise
 * if undefined, "==" is unused
 */
#ifdef GEN_HASH_KEY_CMP
    #define __gh_key_cmp(l,r) (GEN_HASH_KEY_CMP(l,r) == 0)
#else
    #define __gh_key_cmp(l,r) (l == r)
#endif

/*
 * function used to copy keys.
 * function receives object to copy and pointer to location to store the copy
 * should return 1 on success, 0 on failure.
 * if undefined, simple assignment ("=") is used.
 */
#ifdef GEN_HASH_KEY_COPY
    #define __gh_key_copy(target,value) (GEN_HASH_KEY_COPY(value, &target))
#else
    #define __gh_key_copy(target,value) ((target = value), 1)
#endif

#ifdef GEN_HASH_KEY_FREE
    #define __gh_key_free(k) (GEN_HASH_KEY_FREE(k))
#else
    #define __gh_key_free(k)
#endif

/*
 * function used to compare values.
 * should return 0 on equality, non-zero otherwise
 * if undefined, "==" is unused
 */
#ifdef GEN_HASH_VALUE_CMP
    #define __gh_value_cmp(l,r) (GEN_HASH_VALUE_CMP(l,r) == 0)
#else
    #define __gh_value_cmp(l,r) (l == r)
#endif

/*
 * function used to copy values.
 * function receives object to copy and pointer to location to store the copy
 * should return 1 on success, 0 on failure.
 * if undefined, simple assignment ("=") is used.
 */
#ifdef GEN_HASH_VALUE_COPY
    #define __gh_value_copy(target,value) (GEN_HASH_VALUE_COPY(value, &target))
#else
    #define __gh_value_copy(target,value) ((target = value), 1)
#endif

#ifdef GEN_HASH_VALUE_FREE
    #define __gh_value_free(v) (GEN_HASH_VALUE_FREE(v))
#else
    #define __gh_value_free(v)
#endif

/*
 * End Configuration
 */
 
#define GEN_HASH_DECLARE_STORAGE(type, key_t, value_t) \
    typedef struct type##_node type##_node_t; \
    struct type##_node { \
        key_t           key; \
        value_t         value; \
    }; \
    \
    typedef struct type { \
        gh_hash_t       n_buckets; \
        gh_hash_t       n_occupied; \
        gh_hash_t       upper_bound; \
        gh_hash_t       size; \
        unsigned char   *flags; \
        type##_node_t   *buckets; \
    } type##_t;
    
#define GEN_HASH_DECLARE_INTERFACE(type, key_t, value_t) \
    int         type##_init(type##_t *hsh); \
    gh_hash_t   type##_find_slot(type##_t *hsh, key_t k); \
    int         type##_contains(type##_t *hsh, key_t k); \
    int         type##_read(type##_t *hsh, key_t k, value_t *v); \
    int         type##_put(type##_t *hsh, key_t k, value_t v); \
    int         type##_delete(type##_t *hsh, key_t k); \
    gh_hash_t   type##_size(type##_t *hsh);
    
#define GEN_HASH_DECLARE_STATIC_INTERFACE(type, key_t, value_t) \
    static int          type##_init(type##_t *hsh); \
    static gh_hash_t    type##_find_slot(type##_t *hsh, key_t k); \
    static int          type##_contains(type##_t *hsh, key_t k); \
    static int          type##_read(type##_t *hsh, key_t k, value_t *v); \
    static int          type##_put(type##_t *hsh, key_t k, value_t v); \
    static int          type##_delete(type##_t *hsh, key_t k); \
    static gh_hash_t    type##_size(type##_t *hsh);

#define GEN_HASH_INIT(type, key_t, value_t) \
    static const gh_hash_t type##_primes[] = { \
        GEN_HASH_BUCKET_SIZES\
    }; \
    static const gh_hash_t type##_n_primes = (sizeof(type##_primes)/sizeof(gh_hash_t)); \
    \
    int __##type##_resize(type##_t *hsh, gh_hash_t new_buckets) { \
        unsigned char *new_flags = NULL; \
        gh_hash_t pix = type##_n_primes - 1; \
        while (type##_primes[pix] > new_buckets) pix--; \
        new_buckets = type##_primes[pix+1]; \
        /* this is in attractivechaos's original but not sure why; \
         * calling resize is based on hash occupancy, not size, so \
         * why use size here? i'll leave this code here as a \
         * reminder to muse over it every now and again... */ \
        /* if (h->size >= (new_buckets * GEN_HASH_MAX_LOAD + 0.5)) return 1; */ \
        gh_hash_t new_flags_size = sizeof(unsigned char) * ((new_buckets >> 2) + 1); \
        new_flags = __gh_malloc(new_flags_size); \
        if (!new_flags) return 0; \
        memset(new_flags, 0, new_flags_size); \
        if (new_buckets > hsh->n_buckets) { \
            hsh->buckets = __gh_realloc(hsh->buckets, new_buckets * sizeof(type##_node_t)); \
            if (!hsh->buckets) { \
                __gh_free(new_flags); \
                return 0; \
            } \
        } \
        int j; \
        for (j = 0; j < hsh->n_buckets; j++) { \
            if (GH_BUCKET_STATE(hsh->flags, j) == GH_BUCKET_FULL) { \
                key_t key = hsh->buckets[j].key; \
                value_t val = hsh->buckets[j].value; \
                GH_SET_BUCKET_STATE(hsh->flags, j, GH_BUCKET_DELETED); \
                while (1) { \
                    gh_hash_t hc    = __gh_hash_key(key); \
                    gh_hash_t hb    = hc % new_buckets; \
                    gh_hash_t inc   = 1 + hc % (new_buckets - 1); \
                    while (GH_BUCKET_STATE(new_flags, hb) != GH_BUCKET_EMPTY) { \
                        hb += inc; \
                        if (hb >= new_buckets) hb -= new_buckets; \
                    } \
                    GH_SET_BUCKET_STATE(new_flags, hb, GH_BUCKET_FULL); \
                    if (hb < hsh->n_buckets && GH_BUCKET_STATE(hsh->flags, hb) == GH_BUCKET_FULL) { \
                        key_t tmp_key = hsh->buckets[hb].key; \
                        value_t tmp_val = hsh->buckets[hb].value; \
                        hsh->buckets[hb].key = key; \
                        hsh->buckets[hb].value = val; \
                        key = tmp_key; \
                        val = tmp_val; \
                        GH_SET_BUCKET_STATE(hsh->flags, hb, GH_BUCKET_DELETED); \
                    } else { \
                        hsh->buckets[hb].key = key; \
                        hsh->buckets[hb].value = val; \
                        break; \
                    } \
                } \
            } \
        } \
        \
        if (new_buckets < hsh->n_buckets) { \
            hsh->buckets = __gh_realloc(hsh->buckets, new_buckets * sizeof(type##_node_t)); \
            if (!hsh->buckets) { \
                __gh_free(new_flags); \
                return 0; \
            } \
        } \
        __gh_free(hsh->flags); \
        hsh->flags = new_flags; \
        hsh->n_buckets = new_buckets; \
        hsh->n_occupied = hsh->size; \
        hsh->upper_bound = hsh->n_buckets * GEN_HASH_MAX_LOAD + 0.5; \
        return 1; \
    } \
    \
    int type##_init(type##_t *hsh) { \
        memset(hsh, 0, sizeof(type##_t)); \
        return 1; \
    } \
    \
    gh_hash_t type##_find_slot(type##_t *hsh, key_t k) { \
        if (hsh->n_buckets) { \
            gh_hash_t hc    = __gh_hash_key(k); \
            gh_hash_t hb    = hc % hsh->n_buckets; \
            gh_hash_t inc   = 1 + hc % (hsh->n_buckets - 1); \
            gh_hash_t last  = hb; \
            while (1) { \
                char state = GH_BUCKET_STATE(hsh->flags, hb); \
                if (state == GH_BUCKET_EMPTY) { \
                    break; \
                } else if (state == GH_BUCKET_FULL && __gh_key_cmp(hsh->buckets[hb].key, k)) { \
                    return hb; \
                } \
                hb += inc; \
                if (hb >= hsh->n_buckets) hb -= hsh->n_buckets; \
                if (hb == last) break; \
            } \
        } \
        return hsh->n_buckets; \
    } \
    \
    int type##_read(type##_t *hsh, key_t k, value_t *v) { \
        gh_hash_t slot = type##_find_slot(hsh, k); \
        if (slot == hsh->n_buckets) { \
            return 0; \
        } else { \
            *v = hsh->buckets[slot].value; \
            return 1; \
        } \
    } \
    \
    int type##_put(type##_t *hsh, key_t k, value_t v) { \
        \
        gh_hash_t sz_before; \
        if (__gh_debug) sz_before = hsh->size; \
        \
        if (hsh->n_occupied >= hsh->upper_bound) { \
            if (!__##type##_resize(hsh, hsh->n_buckets + ((hsh->n_buckets > (hsh->size * 2)) ? -1 : 1))) { \
                return 0; \
            } \
        } \
        \
        gh_hash_t hc    = __gh_hash_key(k); \
        gh_hash_t hb    = hc % hsh->n_buckets; \
        gh_hash_t inc   = 1 + hc % (hsh->n_buckets - 1); \
        gh_hash_t tgt   = hsh->n_buckets; \
        gh_hash_t old   = hsh->n_buckets; \
        \
        while (1) { \
            char state = GH_BUCKET_STATE(hsh->flags, hb); \
            if (state == GH_BUCKET_EMPTY) { \
                if (tgt == hsh->n_buckets) tgt = hb; \
                break; /* search is over; this key can't exist anywhere else */ \
            } else if (state == GH_BUCKET_DELETED) { \
                if (tgt == hsh->n_buckets) tgt = hb; \
            } else if (__gh_key_cmp(hsh->buckets[hb].key, k)) { \
                old = hb; \
                if (tgt == hsh->n_buckets) tgt = hb; \
                break; \
            } \
            hb += inc; \
            if (hb >= hsh->n_buckets) hb -= hsh->n_buckets; \
        } \
        \
        if (old != hsh->n_buckets) { /* replace */ \
            __gh_value_free(hsh->buckets[old].value); \
            if (!__gh_value_copy(hsh->buckets[tgt].value, v)) { \
                return 0; \
            } \
            if (old != tgt) { \
                if (!__gh_key_copy(hsh->buckets[tgt].key, k)) { \
                    return 0; \
                } \
                GH_SET_BUCKET_STATE(hsh->flags, old, GH_BUCKET_DELETED); \
                GH_SET_BUCKET_STATE(hsh->flags, tgt, GH_BUCKET_FULL); \
            } \
        } else { /* insert */ \
            hsh->size++; \
            if (GH_BUCKET_STATE(hsh->flags, tgt) == GH_BUCKET_EMPTY) hsh->n_occupied++; \
            if (__gh_key_copy(hsh->buckets[tgt].key, k) && __gh_value_copy(hsh->buckets[tgt].value, v)) { \
                GH_SET_BUCKET_STATE(hsh->flags, tgt, GH_BUCKET_FULL); \
            } else { \
                return 0; \
            } \
        } \
        \
        return 1; \
    } \
    \
    int type##_delete(type##_t *hsh, key_t k) { \
        \
        gh_hash_t sz_before; \
        if (__gh_debug) sz_before = hsh->size; \
        \
        gh_hash_t slot = type##_find_slot(hsh, k); \
        if (slot == hsh->n_buckets) { \
            if (__gh_debug) assert(hsh->size == sz_before); \
            return 0; \
        } else { \
            \
            if (__gh_debug) { \
                assert(__gh_key_cmp(k, hsh->buckets[slot].key)); \
                assert(GH_BUCKET_STATE(hsh->flags, slot) == GH_BUCKET_FULL); \
            } \
            \
            __gh_key_free(hsh->buckets[slot].key); \
            __gh_value_free(hsh->buckets[slot].value); \
            GH_SET_BUCKET_STATE(hsh->flags, slot, GH_BUCKET_DELETED); \
            hsh->size--; \
            \
            if (__gh_debug) { \
                assert(hsh->size == sz_before - 1); \
                assert(GH_BUCKET_STATE(hsh->flags, slot) == GH_BUCKET_DELETED); \
            } \
            \
            return 1; \
            \
        } \
        \
    } \
    \
    gh_hash_t type##_size(type##_t *hsh) { \
        return hsh->size; \
    } \
    
#define GEN_HASH_DECLARE(type, key_t, value_t) \
    GEN_HASH_DECLARE_STORAGE(type, key_t, value_t); \
    GEN_HASH_DECLARE_INTERFACE(type, key_t, value_t);

#define GEN_HASH(type, key_t, value_t) \
    GEN_HASH_DECLARE(type, key_t, value_t); \
    GEN_HASH_INIT(type, key_t, value_t); \

