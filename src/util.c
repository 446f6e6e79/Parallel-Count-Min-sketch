#include <headers/util.h>

/*
    Safely frees all allocated memory and closes MPI file handles if valid.
*/
void safe_cleanup(uint8_t *buffer, CountMinSketch *cms, MPI_File *fh) {
    if (buffer)
        free(buffer);
    if (cms)
        cms_free(cms);
    if (fh && *fh != MPI_FILE_NULL)
        MPI_File_close(fh);
    MPI_Finalize();
}

/*
    Convert an IPv4-mapped address to a 32-bit integer
*/
uint32_t ip_to_int(const uint8_t *ip_addr) {
    // Assuming ip_addr is a valid IPv4-mapped  address
    return (uint32_t)(ip_addr[12] << 24 | ip_addr[13] << 16 | ip_addr[14] << 8 | ip_addr[15]);
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