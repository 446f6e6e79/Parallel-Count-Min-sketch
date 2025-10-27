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