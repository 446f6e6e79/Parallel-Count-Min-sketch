#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <mpi.h>

typedef struct {
    int width;       // columns
    int depth;       // rows
    uint32_t **table;   // 2D array for counts
    uint32_t *hash_a;   // Universal hash function parameters: we need two hash seeds
    uint32_t *hash_b;
    uint32_t prime;     // A large prime number for hashing
} CountMinSketch;

#endif