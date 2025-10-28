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


*/
void cms_debug_print(CountMinSketch *cms) {
    int count = 0;
    for (int row = 0; row < cms->depth; row++) {
        for (int col = 0; col < cms->width; col++) {
            if(cms->table[row * cms->width + col] > 0)
                count += cms->table[row * cms->width + col]; 
        }
    }
    printf("There are %d occurences\n", count);
}



/*
    Batch update the Count-Min-Sketch table for an array of keys
*/
void cms_batch_update(CountMinSketch *cms, const uint8_t *keys, size_t n_keys) {
    for (size_t i = 0; i < n_keys; i++) {
        uint32_t key_int = ip_to_int(&keys[i * IP_SIZE]); 
        cms_update(cms, key_int);
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

/*
    Convert an IPv4-mapped address to a 32-bit integer
*/
uint32_t ip_to_int(const uint8_t *ip_addr) {
    // Assuming ip_addr is a valid IPv4-mapped IPv6 address
    return (uint32_t)(ip_addr[12] << 24 | ip_addr[13] << 16 | ip_addr[14] << 8 | ip_addr[15]);
}