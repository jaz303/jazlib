#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#include "jazlib/common.h"

#include "jazlib/gen_vector_reset.h"
#define JAZLIB_DEBUG
#define GEN_VECTOR_GROW_FACTOR 2
#include "jazlib/gen_vector.h"
GEN_VECTOR(vector, int);

int main(int argc, char *argv[]) {
    
    vector_t vec;
    vector_init(&vec);
    
    printf("vector initialised\n");
    printf("vector size=%d\n", vec.size);
    printf("vector capacity=%d\n", vec.capacity);
    
    int i;
    for (i = 0; i < 50; i++) {
        vector_set(&vec, i * 2, i);
    }
    
    printf("vector size=%d\n", vec.size);
    printf("vector capacity=%d\n", vec.capacity);
    
    while (vec.size > 10) {
        int v = vector_get(&vec, 0);
        vector_delete(&vec, 0);
    }
    
    printf("vector size=%d\n", vec.size);
    printf("vector capacity=%d\n", vec.capacity);
    
    vector_set(&vec, vec.size, 100);
    
    printf("vector size=%d\n", vec.size);
    printf("vector capacity=%d\n", vec.capacity);
    
    return 0;

}