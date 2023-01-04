#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

typedef struct initial {
    int first;
    int second;
} INITIAL;

void myMax(INITIAL *inbuf, INITIAL *outbuf, int *len, MPI_Datatype type)
{
    *outbuf = (*inbuf).first > (*outbuf).first ? *inbuf : *outbuf;
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Define a new type
    MPI_Datatype MPI_INITIAL;
    
    int count = 1;
    int lengths[1] = { 2 };
    MPI_Aint displacements[1] = { 0 };
    MPI_Datatype types[1] = { MPI_INT };

    MPI_Type_create_struct(count, lengths, displacements, types, &MPI_INITIAL);
    MPI_Type_commit(&MPI_INITIAL);

    // Test a new type
    INITIAL test;
    INITIAL matrix[10][10];

    if (rank == 0)
    {
        test.first = 1;
        test.second = 2;
    }

    if (rank == 0)
    {
        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                matrix[i][j].first = i+j;
                matrix[i][j].second = i*j;
            }
        }
    }

    MPI_Bcast(&test, 1, MPI_INITIAL, 0, MPI_COMM_WORLD);
    MPI_Bcast(matrix, 10 * 10, MPI_INITIAL, 0, MPI_COMM_WORLD);

        printf("Rank[%d]: first: %d, second: %d\n", rank, test.first, test.second);

        printf("RANK: %d\n", rank);
        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                matrix[i][j].first = i+j + rank;
                matrix[i][j].second = i*j * rank;

                printf("[%3d %3d] ", matrix[i][j].first, matrix[i][j].second);
            }
            printf("\n");
        }
        

    // Create reduction operation
    MPI_Op MPI_MY_MAX;
    MPI_Op_create((MPI_User_function *)myMax, 1, &MPI_MY_MAX);

    INITIAL max;
    INITIAL localMax = matrix[0][0];
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (matrix[i][j].first > localMax.first)
            {
                localMax = matrix[i][j];
            }
        }
    }
    MPI_Reduce(&localMax, &max, 1, MPI_INITIAL, MPI_MY_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("MAX: [%3d %3d]\n", max.first, max.second);
    }

    MPI_Finalize();

    return 0;
}