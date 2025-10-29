#ifndef CMS_H
#define CMS_H

#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define IP_SIZE 4 // Each IPv4 address is 4 bytes

typedef struct {
    int width;       // columns
    int depth;       // rows
    uint32_t *table;   // 2D array for counts
    uint32_t *hash_a;   // Universal hash function parameters: we need two hash seeds
    uint32_t *hash_b;
    uint32_t prime;     // A large prime number for hashing
} CountMinSketch;

// Functions to create and free Count-Min Sketch
CountMinSketch *cms_create(int width, int depth);
CountMinSketch *cms_create_from_error(double eps, double delta);
void cms_free(CountMinSketch *cms);

// Update and query functions
void cms_update(CountMinSketch *cms, uint32_t key);
void cms_batch_update(CountMinSketch *cms, const uint8_t *keys, size_t n_keys);
uint32_t cms_query(const CountMinSketch *cms, uint32_t key);

// Merge two Count-Min Sketches (src into dest)
void cms_merge_into(CountMinSketch *dest, const CountMinSketch *src);

// Debug function to print the Count-Min Sketch table occurrences
void cms_debug_print(CountMinSketch *cms);


#endif