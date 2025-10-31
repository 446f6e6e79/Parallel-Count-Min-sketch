#include "headers/file_io.h"
/*
    Read IP addresses from a binary file using MPI I/O
    The function takes as input:
        fh: MPI file handle
        buffer: pre-allocated buffer to store the read addresses
        start_index: the starting index (in number of addresses) to read from
        count: number of addresses to read
    The function returns the number of addresses read, or -1 on error.
*/

int read_buffer(MPI_File fh, uint8_t *buffer, MPI_Offset start_index, MPI_Offset count) {
    // Ensure we do not read more than the buffer can hold
    if (count > BUFFER_IP_COUNT) {
        fprintf(stderr, "Error: trying to read %lld addresses but buffer can hold only %d\n",
                (long long)count, BUFFER_IP_COUNT);
        return -1;
    }

    // Calculate byte offset from the start of the file
    MPI_Offset offset = start_index * IP_SIZE;
    MPI_Offset bytes_to_read = count * (MPI_Offset)IP_SIZE;

    // Read this process's local_addresses
    MPI_Status status;
    MPI_File_read_at(fh, offset, buffer, bytes_to_read, MPI_BYTE, &status);

    // Compute how many bytes were actually read
    int bytes_read = 0;
    MPI_Get_count(&status, MPI_BYTE, &bytes_read);
    
    if (bytes_read != bytes_to_read) {
        fprintf(stderr, "Error: Process failed to read the expected number of bytes. Expected %lld, got %d\n", bytes_to_read, bytes_read);
        return -1;
    }
    return (int)(bytes_read / IP_SIZE); // Return number of addresses read
}


int non_blocking_read_buffer(MPI_File fh, uint8_t *buffer, MPI_Offset start_index, MPI_Offset count, MPI_Request *request) {
    // Ensure we do not read more than the buffer can hold
    if (count > BUFFER_IP_COUNT) {
        fprintf(stderr, "Error: trying to read %lld addresses but buffer can hold only %d\n",
                (long long)count, BUFFER_IP_COUNT);
        return -1;
    }

    // Calculate byte offset from the start of the file
    MPI_Offset offset = start_index * IP_SIZE;
    MPI_Offset bytes_to_read = count * (MPI_Offset)IP_SIZE;

    // Implement non-blocking read
    MPI_File_iread_at_all(fh, offset, buffer, bytes_to_read, MPI_BYTE, request);
    
    return 0; // Return 0 to indicate the read has been initiated
}
/*
    Write the execution information to the output csv file
    The file format is:
        n_process,total_size,time_seconds
    Each execution will append a new line to the file.
*/
int write_execution_info(const char *filename, int n_process, MPI_Offset n_elements, double time_seconds, double io_time, double compute_time, double busy_wait_time) {
    // Open the file in append mode
    FILE *fp = fopen(filename, "a");
    if (fp == NULL) {
        fprintf(stderr, "File size is not a multiple of IP address size\n");
        return -1;
    }

    // Getting the POSIX file descriptor from the FILE pointer
    int fd = fileno(fp);
    if (fd == -1) {
        fprintf(stderr, "Failed to get file descriptor\n");
        fclose(fp);
        return -1;
    }
    // Lock the file for exclusive access
    if (flock(fd, LOCK_EX) == -1) {
        fprintf(stderr, "Failed to lock file\n");
        fclose(fp);
        return -1;
    }

    // Write the execution info (note: MPI_Offset is typically a long long)
    if (fprintf(fp, "%d,%lld,%.6f,%.6f,%.6f,%.6f\n", n_process, (long long)n_elements, time_seconds, io_time, compute_time, busy_wait_time) == -1) {
        fprintf(stderr, "Failed to write to file\n");

        flock(fd, LOCK_UN);
        fclose(fp);
        return -1;
    }

    // Flush to ensure the data is actually written
    fflush(fp);
    
    // Release lock and close the file
    flock(fd, LOCK_UN);       
    fclose(fp);
    return 0;
}