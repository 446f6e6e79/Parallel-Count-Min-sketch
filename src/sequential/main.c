#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    int width;       // columns
    int depth;       // rows
    uint32_t **table;   // 2D array for counts
    uint32_t *hash_a;   // Universal hash function parameters: we need two hash seeds
    uint32_t *hash_b;
    uint32_t prime;     // A large prime number for hashing
} CountMinSketch;

/*
    * Initialize Count-Min Sketch data structure
    https://github.com/barrust/count-min-sketch/blob/master/src/count_min_sketch.c
    http://www.isthe.com/chongo/tech/comp/fnv/  TO TEST HASH
    https://github.com/alabid/countminsketch/blob/master/count_min_sketch.cpp# CHOOSED HASH
*/

/*
    Initialize Count-Min Sketch structure
*/
CountMinSketch* cms_init(int width, int depth) {
    CountMinSketch *cms = malloc(sizeof(CountMinSketch));
    cms->width = width;
    cms->depth = depth;
    // Use a large prime number less than 2^32
    cms->prime = 4294967291U;

    // Allocate 2D table
    cms->table = malloc(depth * sizeof(uint32_t*));
    for (int i = 0; i < depth; i++)
        cms->table[i] = calloc(width, sizeof(uint32_t));

    // Allocate hash function parameters
    cms->hash_a = malloc(depth * sizeof(uint32_t));
    cms->hash_b = malloc(depth * sizeof(uint32_t));
    srand(0); // Seed for reproducibility
    // Initialize hash function arrays
    for (int i = 0; i < depth; i++) {
        cms->hash_a[i] = rand() % cms->prime;
        cms->hash_b[i] = rand() % cms->prime;
    }

    return cms;
}

/*
    Universal hash function for Count-Min Sketch
*/
uint32_t cms_hash(CountMinSketch *cms, uint32_t ip, int row) {
    // hash_a * item + hash_b mod prime mod width
    return ((cms->hash_a[row] * ip + cms->hash_b[row]) % cms->prime) % cms->width;
}

/*
    Update Count-Min Sketch with an item
*/
void cms_update(CountMinSketch *cms, uint32_t ip) {
    for (int i = 0; i < cms->depth; i++) {
        int col = cms_hash(cms, ip, i);
        cms->table[i][col]++;
    }
}

/*  
    Query the estimated the minimum count of an item
*/
uint32_t cms_estimate(CountMinSketch *cms, uint32_t ip) {
    uint32_t min_count = UINT32_MAX;
    for (int i = 0; i < cms->depth; i++) {
        int col = cms_hash(cms, ip, i);
        if (cms->table[i][col] < min_count)
            min_count = cms->table[i][col];
    }
    return min_count;
}

/*
    Free memory allocated for Count-Min Sketch
*/
void cms_free(CountMinSketch *cms) {
    for (int i = 0; i < cms->depth; i++)
        free(cms->table[i]);
    free(cms->table);
    free(cms->hash_a);
    free(cms->hash_b);
    free(cms);
}


int main(int argc, char **argv) {
    // Read binary file containing IPv4 addresses
    const char *filename = "../../data/data.bin";
    printf("File: %s", filename);
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
    CountMinSketch *cms = cms_init(5000, 5);

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

    cms_free(cms);
    fclose(fp);
    return 0;
}
