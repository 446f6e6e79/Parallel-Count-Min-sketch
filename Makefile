# MPI Compiler settings
MPICC = mpicc
MPIFLAGS = -Wall -O2

# Load the module for MPI if needed
module load mpich-3.2

# Program name
TARGET = mpi_program

# Source files (add your .c files here)
SRC = main.c

