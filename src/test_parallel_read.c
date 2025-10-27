#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    MPI_File fh;
    MPI_Status status;
    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_File_open(MPI_COMM_WORLD, "/home/davide.dona-1/Parallel-Count-Min-sketch/data/data.bin", MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);

    // Each IPv4 is 4 bytes
    int ipv4_size = 4;

    // Determine total number of addresses (optional, if known)
    MPI_Offset file_size;
    MPI_File_get_size(fh, &file_size);
    printf("File zise: %d", file_size);
    int total_addresses = file_size / ipv4_size;

    // Divide work among processes
    int local_size = total_addresses / size;
    int remainder = total_addresses % size;

    int start_index = rank * local_size;
    int count = local_size;
    if(rank < remainder){
        start_index += rank;
        count += 1;   
    }

    // Buffer to hold this process's IPs
    uint8_t *buffer = malloc(count * ipv4_size);

    // Calculate byte offset
    MPI_Offset offset = (MPI_Offset)start_index * ipv4_size;

    // Read this process's local_size
    MPI_File_read_at(fh, offset, buffer, count * ipv4_size, MPI_BYTE, &status);

    // Print a few results
    for (int i = 0; i < count; i++) {
        printf("Rank %d read IP: %u.%u.%u.%u\n", rank,
               buffer[i * 4 + 0],
               buffer[i * 4 + 1],
               buffer[i * 4 + 2],
               buffer[i * 4 + 3]);
    }

    free(buffer);
    MPI_File_close(&fh);
    MPI_Finalize();
    return 0;
}