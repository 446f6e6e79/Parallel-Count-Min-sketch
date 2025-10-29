#include "headers/main.h"
/*
    * Main function for parallel Count-Min Sketch using MPI
    The program reads IP addresses from a binary file in parallel,
    updates a Count-Min Sketch data structure, and prints debugging information.
    Usage: mpirun -n <num_processes> ./CMsketch <input_file> <output_file> <epsilon> <delta>
*/

int main(int argc, char **argv) {

    // Check the validity of command-line arguments
    if (argc < 5) {
        fprintf(stderr, "Usage: %s <input_file> <output_file> <epsilon> <delta>\n", argv[0]);
        return -1;
    }
    if (access(argv[1], F_OK) == -1) {
        fprintf(stderr, "Input file %s does not exist.\n", argv[1]);
        return -1;
    }
    if (access(argv[2], F_OK) == -1) {
        fprintf(stderr, "Output file %s does not exist.\n", argv[1]);
        return -1;
    }
    
    /* Parse and validate epsilon and delta */
    char *endptr;
    errno = 0;
    double epsilon = strtod(argv[3], &endptr);
    if (endptr == argv[3] || *endptr != '\0') {
        fprintf(stderr, "Invalid epsilon: '%s' (not a number)\n", argv[3]);
        return -1;
    }

    errno = 0;
    double delta = strtod(argv[4], &endptr);
    if (endptr == argv[4] || *endptr != '\0') {
        fprintf(stderr, "Invalid delta: '%s' (not a number)\n", argv[4]);
        return -1;
    }

    // Initialize MPI environment
    MPI_Init(&argc, &argv);

    MPI_File fh;    // MPI file handle 
    int rank, size; // MPI rank (current process) and size (number of processes)
    double start_time, end_time;

    // Initialize MPI communication, getting rank and size
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    start_time = MPI_Wtime();

    MPI_Offset file_size = 0;

    // Open the binary file containing IPv4 addresses
    MPI_File_open(MPI_COMM_WORLD, argv[1], MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
    if(fh == MPI_FILE_NULL) {
        fprintf(stderr, "Error opening file %s\n", argv[1]);
        MPI_Finalize();
        return -1;
    }

    // Get the size of the file
    MPI_File_get_size(fh, &file_size);
    if(file_size % IP_SIZE != 0) {
        if (rank == 0) {
            fprintf(stderr, "File size is not a multiple of IP address size\n");
        }
        MPI_File_close(&fh);
        MPI_Finalize();
        return -1;
    }

    /*
        Compute the portion of the file each MPI process should handle.

        total_addresses: total number of data elements in the file (file_size divided by size of each element)
        base_address: number of elements each process gets in an even distribution
        remainder: leftover elements that don't fit evenly
        start_index: the starting index for this process. Processes with rank < remainder
                     get one extra element to distribute the leftovers fairly.
        count: total number of elements this process will handle, which is base_address
               plus one if the rank is less than remainder (to account for uneven division).
    */
    MPI_Offset total_addresses = file_size / IP_SIZE;
    MPI_Offset base_address = total_addresses / size;
    MPI_Offset remainder = total_addresses % size;
    MPI_Offset start_index = (MPI_Offset)rank * base_address + (rank < remainder ? rank : remainder);
    MPI_Offset count = base_address + (rank < remainder ? 1 : 0);

    // Allocate buffer to read IP addresses. Each process has its own buffer.
    uint8_t *buffer = malloc(count * IP_SIZE);
    if(!buffer) {
        fprintf(stderr, "Error allocating memory for buffer on rank %d\n", rank);
        MPI_File_close(&fh);
        MPI_Finalize();
        return -1;
    }
    // TO DO: enormous buffer handler
    // Read the assigned portion of the file into the buffer
    int addresses_read = read_buffer(fh, buffer, start_index, count);
    if (addresses_read == -1) { 
        fprintf(stderr, "Error reading buffer on rank %d\n", rank);
        MPI_File_close(&fh);
        free(buffer);
        MPI_Finalize();
        return -1;
    }

    // Initialize Count-Min Sketch
    CountMinSketch *cms = cms_create_from_error(epsilon, delta);
    
    // Batch update Count-Min Sketch with the read IP addresses
    cms_batch_update(cms, buffer, addresses_read);

    end_time = MPI_Wtime();

    // Debugging prints
    printf("Rank %d processed %d addresses.\n", rank, addresses_read);
    cms_debug_print(cms);

    // Cleanup
    free(buffer);
    MPI_File_close(&fh);
    cms_free(cms);

    //Log the info about the runtime
    if(rank == 0){
        write_execution_info(argv[2], size, total_addresses, end_time - start_time);
    }
    
    MPI_Finalize();
    return 0;

}
