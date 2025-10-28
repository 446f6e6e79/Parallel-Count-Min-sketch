#include "cms.h"

/* 
    Create a Count-Min Sketch from the given error parameters
   - epsilon: The desired error rate (probabilistic)
   - delta: The desired confidence level (probabilistic)
*/
CountMinSketch *cms_create_from_error(double epsilon, double delta) {
    if (epsilon <= 0.0 || delta <= 0.0 || delta >= 1.0) return NULL;
    const double E = 2.718281828459045;
    // Number of columns (width)
    int w = (int)ceil(E / epsilon);
    // Number of rows (depth)
    int d = (int)ceil(log(1.0 / delta));
    if (w < 1) w = 1;
    if (d < 1) d = 1;
    // Create the Count-Min Sketch with calculated dimensions
    return cms_create(w, d);
}

/* 
    Create a Count-Min Sketch from the given width and depth parameters
    - width: number of columns
    - depth: number of rows
*/
CountMinSketch *cms_create(int width, int depth) {
    if (width < 1) width = 1;
    if (depth < 1) depth = 1;
    // Allocate memory for CountMinSketch structure
    CountMinSketch *cms = malloc(sizeof(CountMinSketch));
    cms->width = width;
    cms->depth = depth;
    // Allocate table as a contiguous block, initialize to zero (calloc)
    cms->table = calloc(width * depth, sizeof(uint32_t));
    // Allocate hash function parameters
    cms->hash_a = malloc(depth * sizeof(uint32_t));
    cms->hash_b = malloc(depth * sizeof(uint32_t));
    srand(0); // Seed for reproducibility
    // Use a large prime number
    cms->prime = 4294967291U;
    // Initialize hash function arrays
    for (int i = 0; i < depth; i++) {
        cms->hash_a[i] = rand() % cms->prime;
        cms->hash_b[i] = rand() % cms->prime;
    }
    // Create the Count-Min Sketch with calculated dimensions
    return cms;
}

/*
    Hash function for Count-Min Sketch
*/
uint32_t cms_hash(CountMinSketch *cms, uint32_t ip, int row) {
    // hash_a * item + hash_b mod prime mod width
    return ((cms->hash_a[row] * ip + cms->hash_b[row]) % cms->prime) % cms->width;
}

/*
    Function to hash an item using universal hashing and increase count
*/
void cms_update(CountMinSketch *cms, uint32_t ip) {
    for (int row = 0; row < cms->depth; row++) {
        int column = cms_hash(cms, ip, row);
        cms->table[row * cms->width + column]++;
    }
}

/*
    Free memory allocated for Count-Min Sketch
*/
void cms_free(CountMinSketch *cms) {
    free(cms->table);
    free(cms->hash_a);
    free(cms->hash_b);
    free(cms);      
}