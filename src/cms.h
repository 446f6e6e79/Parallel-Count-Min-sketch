#ifndef CMS_H
#define CMS_H

#include <stdint.h>
#include <stddef.h>

typedef struct CountMinSketch CountMinSketch;

// Functions to create and free Count-Min Sketch
CountMinSketch *cms_create(int width, int depth);
CountMinSketch *cms_create_from_error(double eps, double delta);
void cms_free(CountMinSketch *cms);

// Update and query functions
void cms_update(CountMinSketch *cms, uint32_t key, uint32_t count);
uint32_t cms_query(const CountMinSketch *cms, uint32_t key);

// Merge two Count-Min Sketches (src into dest)
void cms_merge_into(CountMinSketch *dest, const CountMinSketch *src);

#endif