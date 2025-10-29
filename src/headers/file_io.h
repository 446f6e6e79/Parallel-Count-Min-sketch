#ifndef FILE_IO_H
#define FILE_IO_H

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/file.h>

#define IP_SIZE 4 // Each IPv4 address is 4 bytes
#define BUFFER_SIZE 1048576 // 1 MB buffer size
#define BUFFER_IPS (BUFFER_BYTES / IP_SIZE)

int read_buffer(MPI_File fh, uint8_t *buffer, MPI_Offset start_index, MPI_Offset count);
int write_execution_info(const char *filename, int n_process, MPI_Offset n_elements, double time_seconds);

#endif
