#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include "mpi.h"

typedef struct distances {
    int first;
    int second;
    int distance;
} DISTANCE;

DISTANCE* AllocateDistances(long n)
{
    DISTANCE* array = (DISTANCE*)malloc(n * sizeof(DISTANCE));
    for (long i = 0; i < n; i++)
    {
        array[i].first = 0;
        array[i].second = 0;
        array[i].distance = 0;
    }
    return array;
}

void DoWork(DISTANCE *distances, int n)
{
    // TODO
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
    
    // Scan and sand the data
    int n;
    DISTANCE *distances;
    if (rank == 0)
    {
        scanf("%d", &n);
        distances = AllocateDistances(n);
        for (int i = 0; i < n; i++) 
        {
            scanf("%d %d %d", &distances[i].first, &distances[i].second, &distances[i].distance);
        }   
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank != 0)
    {
        distances = AllocateDistances(n);
    }
    MPI_Bcast(distances, n, MPI_DISTANCE, 0, MPI_COMM_WORLD);

    // Do Work
    if (rank == 0)
    {
        DoWork(distances, n-1);
    }
    
    MPI_Finalize();
}