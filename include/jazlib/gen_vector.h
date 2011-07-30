/*
 * Generic Vector
 *
 * my_vector.h
 * #include "gen_vector.h"
 * GEN_VECTOR_DECLARE(my_vector, const char *);
 *
 * my_vector.c
 * #include "gen_vector_reset.h"
 * #define GEN_VECTOR_CMP   strcmp
 * #define GEN_VECTOR_COPY  gen_strcpy
 * #define GEN_VECTOR_FREE  free
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

/*
 * function used to compare keys.
 * should return 0 on equality, non-zero otherwise
 * if undefined, "==" is unused
 */
#ifdef GEN_VECTOR_CMP
    #define __gv_cmp(l,r) (GEN_VECTOR_CMP(l,r))
    #define __gv_is_equal(l,r) (GEN_VECTOR_CMP(l,r) == 0)
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
#ifdef GEN_VECTOR_COPY
    #define __gv_value_copy(target,value) (GEN_VECTOR_COPY(value, &target))
#else
    #define __gv_value_copy(target,value) ((target = value), 1)
#endif

#ifdef GEN_VECTOR_FREE
    #define __gv_value_free(k) (GEN_VECTOR_FREE(k))
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
    value_t     type##_get(type##_t *vec, int ix);
    
#define GEN_VECTOR_INIT(type, key_t, value_t) \
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
    size_t type##_size(type##_t *vec) { \
        return vec->size; \
    } \
    \
    int type##_contains(type##_t *vec, value_t value) { \
        return type##_find(vec, value) >= 0; \
    } \
    \
    int type##_find(type##_t *vec, value_t value) { \
        int i;
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
        return vec->values[--vec->size]; \
    } \
    \
    int type##_set(type##_t *vec, int ix, value_t value) { \
        if (vec->capacity < ix + 1) { \
            /* resize */ \
        } \
        __gv_value_copy(vec->values[ix], value); \
        return 1; \
    } \
    \
    value_t type##_get(type##_t *vec, size_t ix) { \
        return vec->values[ix]; \
    } \


#define GEN_VECTOR(type, value_t) \
    GEN_VECTOR_DECLARE(type, value_t); \
    GEN_VECTOR_INIT(type, value_t); \

