/************************************************************************
Example 03: Deadlock example
Show how to create Deadlock using a MPI_Send() and MPI_Recv() routines.
************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    /* Init */
    MPI_Init(&argc, &argv);

    /* Main part */
    int size, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        int numberToReceive;
        int numberToSend = 10;

        printf("RANK {%d}: Receiving a value from {%d}.\n", rank, 1);
        MPI_Recv(&numberToReceive, 1, MPI_INT, 1, 1991, MPI_COMM_WORLD, NULL);
        printf("RANK {%d}: Received a value %d.\n", rank, numberToReceive);

        printf("RANK {%d}: Sending a value %d.\n", rank, numberToSend);
        MPI_Send(&numberToSend, 1, MPI_INT, 1, 2991, MPI_COMM_WORLD);
    }

    if (rank == 1) {
        int numberToReceive;
        int numberToSend = 100;

        printf("RANK {%d}: Receiving a value from {%d}.\n", rank, 0);
        MPI_Recv(&numberToReceive, 1, MPI_INT, 0, 1991, MPI_COMM_WORLD, NULL);
        printf("RANK {%d}: Received a value %d.\n", rank, numberToReceive);


        printf("RANK {%d}: Sending a value %d.\n", rank, numberToSend);
        MPI_Send(&numberToSend, 1, MPI_INT, 0, 2991, MPI_COMM_WORLD);
    }

    /* Finalize */
    MPI_Finalize();
}