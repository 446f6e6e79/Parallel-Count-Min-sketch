#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>


typedef struct {
    int width;       // columns
    int depth;       // rows
    uint32_t *table;   // 2D array for counts
    uint32_t *hash_a;   // Universal hash function parameters: we need two hash seeds
    uint32_t *hash_b;
    uint32_t prime;     // A large prime number for hashing
} CountMinSketch;

// Prototypes
CountMinSketch *cms_create(int width, int depth);
CountMinSketch *cms_create_from_error(double epsilon, double delta);
uint32_t cms_hash(CountMinSketch *cms, uint32_t ip, int row);
void cms_update(CountMinSketch *cms, uint32_t ip);
uint32_t cms_estimate(CountMinSketch *cms, uint32_t ip);
void cms_free(CountMinSketch *cms);
/*
    * Initialize Count-Min Sketch data structure
    https://github.com/barrust/count-min-sketch/blob/master/src/count_min_sketch.c
    http://www.isthe.com/chongo/tech/comp/fnv/  TO TEST HASH
    https://github.com/alabid/countminsketch/blob/master/count_min_sketch.cpp# CHOOSED HASH
*/

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
    if (cms == NULL) {
        fprintf(stderr, "Error: failed to allocate memory for CountMinSketch structure.\n");
        return NULL;
    }
    cms->width = width;
    cms->depth = depth;
    // Allocate the cms table, initialized as zeros
    cms->table = calloc(width * depth, sizeof(uint32_t));
    if (cms->table == NULL) {
        fprintf(stderr, "Error: failed to allocate memory for table.\n");
        free(cms);
        return NULL;
    }
    /* 
        Allocate hash function parameters. Every hash function will use
        2 parameters and the table needs n_rows many hash functions.
        This way we can define a family of pairwise indipendent functions
    */
    cms->hash_a = malloc(depth * sizeof(uint32_t));
    if (cms->hash_a == NULL) {
        fprintf(stderr, "Error: failed to allocate memory for hash_a.\n");
        free(cms->table);
        free(cms);
        return NULL;
    }
    cms->hash_b = malloc(depth * sizeof(uint32_t));
    if (cms->hash_b == NULL) {
        fprintf(stderr, "Error: failed to allocate memory for hash_b.\n");
        free(cms->hash_a);
        free(cms->table);
        free(cms);
        return NULL;
    }
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
    // hash_a * item + hash_b mod prime mod width
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
    Query the estimated the minimum count of an item
*/
uint32_t cms_estimate(CountMinSketch *cms, uint32_t x) {
    uint32_t min_count = UINT32_MAX;
    for (int i = 0; i < cms->depth; i++) {
        int col = cms_hash(cms, x, i);
        if (cms->table[i * cms->width + col] < min_count)
            min_count = cms->table[i * cms->width + col];
    }
    return min_count;
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


int main(int argc, char **argv) {
    // Read binary file containing IPv4 addresses
    const char *filename = "../../data/data.bin";
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("Failed to open file");
        return 1;
    }

    // Each IPv4 is 4 bytes
    const int ipv4_size = 4;
    // Buffer to hold one IPv4 address
    uint8_t buffer[ipv4_size];

    //Create Count-Min Sketch
    CountMinSketch *cms = cms_create_from_error(0.001, 0.99);

    // Iterate through file and read IPv4 addresses
    while (fread(buffer, 1, ipv4_size, fp) == ipv4_size) {
        // Convert bytes to uint32_t representation of IPv4
        uint32_t ip = ((uint32_t)buffer[0] << 24) |
                      ((uint32_t)buffer[1] << 16) |
                       ((uint32_t)buffer[2] << 8) | 
                       ((uint32_t)buffer[3]);
        // Update Count-Min Sketch
        cms_update(cms, ip);
    }
    // Clean up
    cms_free(cms);
    fclose(fp);
    return 0;
}
