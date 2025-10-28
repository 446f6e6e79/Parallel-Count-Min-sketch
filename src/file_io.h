#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define IP_SIZE 4 // Each IPv4 address is 4 bytes
#define BUFFER_SIZE 1048576 // 1 MB buffer size
int read_file(MPI_File fh, int rank, int size);
int write_file(const char *filename, int n_process, size_t total_size, double time_seconds);
