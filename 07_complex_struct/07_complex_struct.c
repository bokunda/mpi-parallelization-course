#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

typedef struct initial {
    float x, y, z;
    float velocity;
    int n, type;
} INITIAL;

void myMax(INITIAL *inbuf, INITIAL *outbuf, int *len, MPI_Datatype type)
{
    *outbuf = (*inbuf).n > (*outbuf).n ? *inbuf : *outbuf;
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Define a new type
    MPI_Datatype MPI_INITIAL;
    
    MPI_Aint lb, extent;
    MPI_Type_get_extent(MPI_FLOAT, &lb, &extent);

    int count = 2;
    int lengths[2] = { 4, 2 };
    MPI_Aint displacements[2] = { 0, 4 * extent };
    MPI_Datatype types[2] = { MPI_FLOAT, MPI_INT };

    MPI_Type_create_struct(count, lengths, displacements, types, &MPI_INITIAL);
    MPI_Type_commit(&MPI_INITIAL);

    // Test a new type
    INITIAL test;
    INITIAL matrix[10][10];

    if (rank == 0)
    {
        test.x = 1.1;
        test.y = 2.2;
        test.z = 3.3;
        test.n = 4;
        test.type = 5;
    }

    if (rank == 0)
    {
        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                matrix[i][j].x = 1.1 + i + j;
                matrix[i][j].y = 2.2 + i + j;
                matrix[i][j].z = 3.3 + i + j;
                matrix[i][j].n = i+j;
                matrix[i][j].type = i*j;
            }
        }
    }

    MPI_Bcast(&test, 1, MPI_INITIAL, 0, MPI_COMM_WORLD);
    MPI_Bcast(matrix, 10 * 10, MPI_INITIAL, 0, MPI_COMM_WORLD);

        printf("Rank[%d]: N: %d, TYPE: %d, x: %lf y: %lf z: %lf\n", 
            rank, test.n, test.type, test.x, test.y, test.z);

        printf("RANK: %d\n", rank);
        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                matrix[i][j].x = 1.1 + i + j + rank;
                matrix[i][j].y = 2.2 + i + j + rank;
                matrix[i][j].z = 3.3 + i + j + rank;
                matrix[i][j].n = i+j + rank;
                matrix[i][j].type = i*j * rank;

                printf("Rank[%d]: N: %d, TYPE: %d, x: %lf y: %lf z: %lf\n", 
                    rank, matrix[i][j].n, matrix[i][j].type, matrix[i][j].x, matrix[i][j].y, matrix[i][j].z);
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
            if (matrix[i][j].n > localMax.n)
            {
                localMax = matrix[i][j];
            }
        }
    }
    MPI_Reduce(&localMax, &max, 1, MPI_INITIAL, MPI_MY_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("MAX : Rank[%d]: N: %d, TYPE: %d, x: %lf y: %lf z: %lf\n", 
            rank, max.n, max.type, max.x, max.y, max.z);
    }

    MPI_Finalize();

    return 0;
}