#include "headers/main.h"
/*
    * Main function for parallel Count-Min Sketch using MPI
    The program reads IP addresses from a binary file in parallel,
    updates a Count-Min Sketch data structure, and prints debugging information.
    Usage: mpirun -n <num_processes> ./CMsketch <input_file> <output_file> <epsilon> <delta>
    TODO: 
        1)ADD timing to different part of the code -> we can sum the time spent in each zone
        2)PASS number of CPUS and NODE used, to be able to add id to the output
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
        fprintf(stderr, "Output file %s does not exist.\n", argv[2]);
        return -1;
    }

    /* Parse and validate epsilon and delta */
    char *endptr;
    double epsilon = strtod(argv[3], &endptr);
    if (endptr == argv[3] || *endptr != '\0') {
        fprintf(stderr, "Invalid epsilon: '%s' (not a number)\n", argv[3]);
        return -1;
    }
    double delta = strtod(argv[4], &endptr);
    if (endptr == argv[4] || *endptr != '\0') {
        fprintf(stderr, "Invalid delta: '%s' (not a number)\n", argv[4]);
        return -1;
    }

    // Initialize MPI environment
    MPI_Init(&argc, &argv);

    MPI_File fh;    // MPI file handle 
    int rank, comm_sz; // MPI rank (current process) and size (number of processes)
    double start_time, end_time;

    // Initialize MPI communication, getting rank and size
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    start_time = MPI_Wtime();

    // Open the binary file containing IPv4 addresses
    MPI_File_open(MPI_COMM_WORLD, argv[1], MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
    if(fh == MPI_FILE_NULL) {
        fprintf(stderr, "Error opening file %s\n", argv[1]);
        safe_cleanup(NULL, NULL, NULL);
        return -1;
    }

    MPI_Offset file_size = 0;
    // Get the size of the file
    MPI_File_get_size(fh, &file_size);
    if(file_size % IP_SIZE != 0) {
        if (rank == 0) {
            fprintf(stderr, "File size is not a multiple of IP address size\n");
        }
        safe_cleanup(NULL, NULL, &fh);
        return -1;
    }

    // Compute the information neede for parallel reading on the file

    // Total number of data elements in the file (file_size divided by size of each element)
    MPI_Offset total_addresses = file_size / IP_SIZE;
    // Number of elements each process gets in an even distribution
    MPI_Offset base_address_count = total_addresses / comm_sz;
    // Leftover elements that don't fit evenly
    MPI_Offset remainder = total_addresses % comm_sz;
    // Starting index to read from the file. Add the remainder of other processes as min(rank, remainder)
    MPI_Offset start_index = (MPI_Offset)rank * base_address_count + (rank < remainder ? rank : remainder);
    // Number of addresses handled by the process. if rank < remainder, assign 1 extra element
    MPI_Offset local_addresses = base_address_count + (rank < remainder ? 1 : 0);

    // Allocate buffer to read IP addresses. Each process has its own buffer.
    uint8_t *buffer = malloc(BUFFER_SIZE);
    if(!buffer) {
        fprintf(stderr, "Error allocating memory for buffer on rank %d\n", rank);
        safe_cleanup(NULL, NULL, &fh);
        return -1;
    }

    // Initialize Count-Min Sketch data structure
    CountMinSketch *cms = cms_create_from_error(epsilon, delta);
    
    // Chunked reading and processing
    MPI_Offset total_read = 0;
    while (total_read < local_addresses) {
        // Remaining IPs to read for this process
        MPI_Offset remaining = local_addresses - total_read;
        // Number of IPs to read in this batch (max BUFFER_IP_COUNT)
        MPI_Offset current_count = (remaining > BUFFER_IP_COUNT) ? BUFFER_IP_COUNT : remaining;
        // Starting index for this batch
        MPI_Offset current_start = start_index + total_read;

        // Read the assigned portion of the file into the buffer
        int addresses_read = read_buffer(fh, buffer, current_start, current_count);
        if (addresses_read == -1) {
            fprintf(stderr, "Error reading buffer on rank %d\n", rank);
            safe_cleanup(buffer, cms, &fh);
            return -1;
        }
        cms_batch_update(cms, buffer, addresses_read);
        total_read += addresses_read;
    }

    end_time = MPI_Wtime();

    // Debugging prints
    printf("Rank %d processed %lld addresses.\n", rank, total_read);

    //Log the info about the runtime
    if(rank == 0){
        write_execution_info(argv[2], comm_sz, total_addresses, end_time - start_time);
    }
    
    // Cleanup allocated resources
    safe_cleanup(buffer, cms, &fh);
    return 0;
}
