#include "cms.h"

/* 
    Create a Count-Min Sketch from the given error parameters
   - epsilon: The desired error rate (probabilistic)
   - delta: The desired confidence level (probabilistic)
*/
CountMinSketch * cms_create_from_error(double epsilon, double delta) {
    if (epsilon <= 0.0 || delta <= 0.0 || delta >= 1.0) return NULL;
    const double E = 2.718281828459045;
    // Number of columns (width)
    int w = (int)ceil(E / epsilon);
    // Number of rows (depth)
    int d = (int)ceil(log(1.0 / delta));
    // Create the Count-Min Sketch with calculated dimensions
    return cms_create(w, d);
}

/* 
    Create a Count-Min Sketch from the given width and depth parameters
    - width: number of columns
    - depth: number of rows (hash functions)
*/
CountMinSketch * cms_create(int width, int depth) {
    if (width < 1) width = 1;
    if (depth < 1) depth = 1;
    
    // Allocate memory for CountMinSketch structure
    CountMinSketch *cms = malloc(sizeof(CountMinSketch));
    cms->width = width;
    cms->depth = depth;
    // Allocate the cms table, initialized as zeros
    cms->table = calloc(width * depth, sizeof(uint32_t));
    /* 
        Allocate hash function parameters. Every hash function will use
        2 parameters and the table needs n_rows many hash functions.
        This way we can define a family of pairwise indipendent functions
    */
    cms->hash_a = malloc(depth * sizeof(uint32_t));
    cms->hash_b = malloc(depth * sizeof(uint32_t));
    srand(0);
    // Use a large prime number
    cms->prime = 4294967291U;
    
    // Initialize parameters for all hash functions 
    for (int i = 0; i < depth; i++) {
        cms->hash_a[i] = rand() % cms->prime;
        cms->hash_b[i] = rand() % cms->prime;
    }

    return cms;
}

/*
    Universal Hash function for Count-Min Sketch.
        h_a,b(x) = ax + b
    The function must return an index [0, width - 1]
*/
uint32_t cms_hash(CountMinSketch *cms, uint32_t x, int row) {
    return ((cms->hash_a[row] * x + cms->hash_b[row]) % cms->prime) % cms->width;
}

/*
    Update the Count-Min-Sketch table for the current value x
*/
void cms_update(CountMinSketch *cms, uint32_t x) {
    for (int row = 0; row < cms->depth; row++) {
        int column = cms_hash(cms, x, row);
        cms->table[row * cms->width + column]++;
    }
}

/*
    Free all the memory allocated for Count-Min Sketch
*/
void cms_free(CountMinSketch *cms) {
    free(cms->table);
    free(cms->hash_a);
    free(cms->hash_b);
    free(cms);      
}