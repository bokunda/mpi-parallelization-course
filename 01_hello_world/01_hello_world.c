/********************************************************
Example 01: Hello World
Show basic MPI routines.
- MPI_Init()
- MPI_Comm_rank()
- MPI_Comm_size()
- MPI_Finalize()

And use default communicator:
- MPI_COMM_WORLD

Idea is to send data to all processors using a for loop.
*********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <time.h>

int main(int argc, char** argv)
{
    /* Init */
    MPI_Init(&argc, &argv);

    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        printf("Hello world!\nSize is: %d\n", size);
    }

    /* Finalize */
    MPI_Finalize();

    return 0;
}