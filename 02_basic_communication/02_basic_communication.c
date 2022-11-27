/********************************************************
Example 02: Basic communication
Show how to use MPI_Send() and MPI_Recv() routines.

Idea is to send data to all processors using a for loop.
*********************************************************/

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

    /* Simulate broadcast, rank 0 sends a message to all recipients. */
    /* Generate random number and multiply it by recipient rank */
    if (rank == 0)
    {
        int number = 10;
        for (int i = 1; i < size; i++)
        {
            int numberToSend = number * i;
            printf("RANK {%d}: Sending a value %d.\n", rank, numberToSend);
            MPI_Send(&numberToSend, 1, MPI_INT, i, 1991, MPI_COMM_WORLD);
        }
    }

    /* Receive a value */
    if (rank != 0)
    {
        int receivedNumber;
        MPI_Recv(&receivedNumber, 1, MPI_INT, 0, 1991, MPI_COMM_WORLD, NULL);
        printf("RANK {%d}: Received a value %d.\n", rank, receivedNumber);
    }

    /* Finalize */
    MPI_Finalize();

    return 0;
}