/*
 * Generic Vector
 *
 * my_vector.h
 * #include "gen_vector.h"
 * GEN_VECTOR_DECLARE(my_vector, const char *);
 *
 * my_vector.c
 * #include "gen_vector_reset.h"
 * #define GEN_VECTOR_VALUE_CMP   strcmp
 * #define GEN_VECTOR_VALUE_COPY  gen_strcpy
 * #define GEN_VECTOR_VALUE_FREE  free
 * #include "gen_vector.h"
 * GEN_VECTOR_INIT(my_vector, const char *);
 */

#ifndef __JAZLIB__GEN_VECTOR_H__
#define __JAZLIB__GEN_VECTOR_H__
    #include <string.h>
    #include <assert.h>
    
    #ifdef JAZLIB_DEBUG
        #define __gv_debug 1
    #else
        #define __gv_debug 0
    #endif
#endif

/*
 * Configuration
 * #define these before #include'ing gen_vector.h
 */
 
/* malloc function (for internal data structures only, not values), default is malloc() */
#ifdef GEN_VECTOR_MALLOC
    #define __gv_malloc(sz) (GEN_VECTOR_MALLOC(sz))
#else
    #include <stdlib.h>
    #define __gv_malloc(sz) malloc(sz)
#endif

/* realloc function (for internal data structures only, not values), default is realloc() */
#ifdef GEN_VECTOR_REALLOC
    #define __gv_realloc(ptr, sz) (GEN_VECTOR_REALLOC(ptr, sz))
#else
    #include <stdlib.h>
    #define __gv_realloc(ptr, sz) realloc(ptr, sz)
#endif

/* free function (for internal data structures only, not values), default is free() */
#ifdef GEN_VECTOR_FREE
    #define __gv_free(ptr) (GEN_VECTOR_FREE(ptr))
#else
    #include <stdlib.h>
    #define __gv_free(ptr) free(ptr)
#endif

/* sizes that vector should grow to, default is to start at 16 then double */
#ifndef GEN_VECTOR_SIZES
    #define GEN_VECTOR_SIZES \
        0,          16,         32,         64,         128,        256, \
        512,        1024,       2048,       4096,       8192,       16384, \
        32768,      65536,      131072,     262144,     524288,     1048576, \
        2097152,    4194304,    8388608,    16777216,   33554432,   67108864, \
        134217728,  268435456,  536870912,  1073741824
#endif

/* threshold at which vector should shrink */
#ifndef GEN_VECTOR_SHRINK_THRESHOLD
    #define GEN_VECTOR_SHRINK_THRESHOLD 0.3
#endif

/*
 * function used to compare values.
 * should return 0 on equality, non-zero otherwise
 * if undefined, "==" is unused
 */
#ifdef GEN_VECTOR_VALUE_CMP
    #define __gv_cmp(l,r) (GEN_VECTOR_VALUE_CMP(l,r))
    #define __gv_is_equal(l,r) (GEN_VECTOR_VALUE_CMP(l,r) == 0)
#else
    #define __gv_cmp(l,r) (l-r)
    #define __gv_is_equal(l,r) (l == r)
#endif

/*
 * function used to copy values.
 * function receives object to copy and pointer to location to store the copy
 * should return 1 on success, 0 on failure.
 * if undefined, simple assignment ("=") is used.
 */
#ifdef GEN_VECTOR_VALUE_COPY
    #define __gv_value_copy(target,value) (GEN_VECTOR_VALUE_COPY(value, &target))
#else
    #define __gv_value_copy(target,value) ((target = value), 1)
#endif

#ifdef GEN_VECTOR_VALUE_FREE
    #define __gv_value_free(k) (GEN_VECTOR_VALUE_FREE(k))
#else
    #define __gv_value_free(k)
#endif

/*
 * End Configuration
 */
 
#define GEN_VECTOR_DECLARE(type, value_t) \
    typedef struct type { \
        int                 size; \
        int                 capacity; \
        int                 shrink_threshold; \
        value_t             *values; \
    } type##_t; \
    \
    int         type##_init(type##_t *vec); \
    void        type##_clear(type##_t *vec); \
    int         type##_size(type##_t *vec); \
    int         type##_contains(type##_t *vec, value_t value); \
    int         type##_find(type##_t *vec, value_t value); \
    int         type##_push(type##_t *vec, value_t value); \
    value_t     type##_pop(type##_t *vec); \
    int         type##_set(type##_t *vec, int ix, value_t value); \
    int         type##_delete(type##_t *vec, int ix); \
    value_t     type##_get(type##_t *vec, int ix);
    
#define GEN_VECTOR_INIT(type, value_t) \
    static const int type##_sizes[] = { \
        GEN_VECTOR_SIZES \
    }; \
    static const int type##_n_sizes = (sizeof(type##_sizes)/sizeof(int)); \
    \
    int type##_init(type##_t *vec) { \
        vec->values = NULL; \
        type##_clear(vec); \
        return 1; \
    } \
    \
    void type##_clear(type##_t *vec) { \
        if (vec->values) { \
            int i; \
            for (i = 0; i < vec->size; i++) { \
                __gv_value_free(vec->values[ix]); \
            } \
            __gv_free(vec->values); \
            vec->values = NULL; \
        } \
        vec->size = 0; \
        vec->capacity = 0; \
    } \
    \
    int type##_size(type##_t *vec) { \
        return vec->size; \
    } \
    \
    int type##_contains(type##_t *vec, value_t value) { \
        return type##_find(vec, value) >= 0; \
    } \
    \
    int type##_find(type##_t *vec, value_t value) { \
        int i; \
        for (i = 0; i < vec->size; i++) { \
            if (__gv_is_equal(vec->values[i], value)) return i; \
        } \
        return -1; \
    } \
    \
    int type##_push(type##_t *vec, value_t value) { \
        return type##_set(vec, vec->size, value); \
    } \
    \
    value_t type##_pop(type##_t *vec) { \
        value_t v = vec->values[vec->size - 1]; \
        type##_delete(vec, vec->size - 1); \
        return v; \
    } \
    \
    int type##_set(type##_t *vec, int ix, value_t value) { \
        size_t new_capacity = vec->capacity; \
        if (vec->capacity < ix + 1) { \
            int s_ix = type##_n_sizes - 1; \
            while (s_ix >= 0) { \
                if (type##_sizes[s_ix] <= vec->capacity) { \
                    new_capacity = type##_sizes[s_ix + 1]; \
                    break; \
                } \
                s_ix--; \
            } \
        } else if (vec->capacity > type##_sizes[1] && vec->size < vec->shrink_threshold) { \
            int s_ix = 1; \
            while (s_ix < type##_n_sizes) { \
                if (type##_sizes[s_ix] > vec->size) { \
                    new_capacity = type##_sizes[s_ix]; \
                    break; \
                } \
                s_ix++; \
            } \
        } \
        if (new_capacity != vec->capacity) { \
            vec->values = __gv_realloc(vec->values, sizeof(value_t) * new_capacity); \
            if (!vec->values) return 0; \
            if (new_capacity > vec->capacity) { \
                memset(vec->values + vec->capacity, 0, (new_capacity - vec->capacity) * sizeof(value_t)); \
            } \
            vec->capacity = new_capacity; \
            vec->shrink_threshold = (int)((float)new_capacity * GEN_VECTOR_SHRINK_THRESHOLD); \
        } \
        if (!__gv_value_copy(vec->values[ix], value)) return 0; \
        if (vec->size < ix + 1) vec->size = ix + 1; \
        return 1; \
    } \
    \
    int type##_delete(type##_t *vec, int ix) { \
        if (vec->size == 0 || ix >= vec->size) return 0; \
        __gv_value_free(vec->values[ix]); \
        if (ix < vec->size - 1) { \
            int i; \
            for (i = ix; i < vec->size - 1; i++) { \
                vec->values[i] = vec->values[i + 1]; \
            } \
        } \
        vec->size--; \
        return 1; \
    } \
    \
    value_t type##_get(type##_t *vec, int ix) { \
        return vec->values[ix]; \
    } \


#define GEN_VECTOR(type, value_t) \
    GEN_VECTOR_DECLARE(type, value_t); \
    GEN_VECTOR_INIT(type, value_t); \

