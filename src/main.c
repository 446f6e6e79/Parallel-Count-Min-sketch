#include "main.h"
#include "file_io.h"
#include "cms.h"

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    MPI_File fh;
    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Finalize();
    return 0;

}
