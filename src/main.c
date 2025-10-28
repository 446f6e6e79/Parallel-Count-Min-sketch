#include "main.h"


#define IP_SIZE 16

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    MPI_File fh;
    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Offset offset = 0;
    MPI_Offset file_size = 0;
    MPI_File_get_size(fh, &file_size);

    if(file_size % IP_SIZE != 0) {
        if (rank == 0) {
            fprintf(stderr, "File size is not a multiple of IP address size\n");
        }
        return -1;
    }

    MPI_Offset total_addresses = file_size / IP_SIZE;
    MPI_Offset base_address = total_addresses / size;
    MPI_Offset remainder = total_addresses % size;

    MPI_Offset start_index = (MPI_Offset)rank * base_address + (rank < remainder ? rank : remainder);
    MPI_Offset count = base_address + (rank < remainder ? 1 : 0);

    
    uint8_t *buffer = malloc(count * IP_SIZE);
    if(!buffer) {
        fprintf(stderr, "Error allocating memory for buffer on rank %d\n", rank);
        MPI_Finalize();
        return -1;
    }

    int addresses_read = read_buffer(fh, buffer, start_index, count);

    if (addresses_read == -1) { 
        fprintf(stderr, "Error reading buffer on rank %d\n", rank);
        MPI_Finalize();
        return -1;
    }

    // Initialize Count-Min Sketch
    CountMinSketch *cms = cms_create_from_error(0.01, 0.99);
    
    // It's needed a type conversion for the buffers
    cms_batch_update(cms, buffer, addresses_read);

    // Debugging prints
    printf("Rank %d processed %d addresses.\n", rank, addresses_read);
    cms_debug_print(cms);

    MPI_File_close(&fh);
    cms_free(cms);
    MPI_Finalize();
    return 0;

}
