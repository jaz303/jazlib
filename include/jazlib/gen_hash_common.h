#ifndef __JAZLIB__GEN_HASH_COMMON_H__
#define __JAZLIB__GEN_HASH_COMMON_H__

#include <stdint.h>

#if UINTPTR_MAX == 0xffffffff
    typedef uint32_t            gh_hash_t;
    typedef gh_hash_t           gh_iter_t;
#elif UINTPTR_MAX == 0xffffffffffffffff
    typedef uint64_t            gh_hash_t;
    typedef gh_hash_t           gh_iter_t;
#else
    #error "get a real computer"
#endif
    
#endif