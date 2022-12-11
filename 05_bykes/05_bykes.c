#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

typedef struct distances {
    int first;
    int second;
    int distance;
} DISTANCE;

DISTANCE* AllocateDistances(int n)
{
    return (DISTANCE*)malloc(n * sizeof(DISTANCE));
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // DEFINE NEW TYPE
    MPI_Datatype MPI_DISTANCE;
    int count = 1;
    int lengths[1] = { 3 };
    MPI_Aint displacements[1] = { 0 };
    MPI_Datatype types[1] = { MPI_INT };

    MPI_Type_create_struct(count, lengths, displacements, types, &MPI_DISTANCE);
    MPI_Type_commit(&MPI_DISTANCE);

    int n = 10;

    DISTANCE *distances;
    distances = AllocateDistances(n);
    
    if (rank == 0)
    {
        for (int i = 0; i < n; i++) 
        {
            distances[i].first = i * n + 1;
            distances[i].second = i * n + 2;
            distances[i].distance = i * n + 3;
        }   
    }

    MPI_Bcast(distances, n, MPI_DISTANCE, 0, MPI_COMM_WORLD);

    if (rank == 2)
    {
        for (int i = 0; i < n; i++)
        {
            printf("rank: %d - %d\n", rank, distances[i].distance);
        }
    }
    
    MPI_Finalize();
}