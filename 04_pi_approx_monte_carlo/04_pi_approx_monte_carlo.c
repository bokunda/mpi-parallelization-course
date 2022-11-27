/********************************************************
Example 04: PI approximation using Monte Carlo method

PI approximation is done using a Monte Carrlo method.
m = amount of dots with condtion xi^2 * yi^2 <= 1;
ro = m / n;
Pi = 4 * ro;

Idea is to parallize calculation of m: xi^2 * yi^2 <= 1
and to send localM values to one node.
*********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#define APPROXIMATION_LEVEL 1000000000

int satisfiesGraphicCondition(double x, double y)
{
    return x*x + y*y <= 1;
}

double calculateRo(int m, int n)
{
    return (double)m / n;
}

double calculatePi(int m, int n)
{
    return 4 * calculateRo(m, n);
}

int main(int argc, char **argv)
{

    /* Init */
    MPI_Init(&argc, &argv);

    /* Main part */
    int size, rank;
    time_t t;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    srand(rank);

    // Calculate local m
    int i;
    int localM = 0;
    int m = 0;

    // Parallelization part _> we have to calculate
    // APPROXIMATION_LEVEL amount of dots, so we can 
    // split that into APPROXIMATION_LEVEL / size batches
    // and calculate a local sum of m.
    for (i = 0; i < APPROXIMATION_LEVEL / size; i++)
    {
        // rand() returns int in interval 0-RAND_MAX
        double x = (double)rand() / RAND_MAX;
        double y = (double)rand() / RAND_MAX;

        localM += satisfiesGraphicCondition(x, y);
    }

    // Send localM to rank 0
    if (rank != 0)
    {
        MPI_Send(&localM, 1, MPI_INT, 0, 1991, MPI_COMM_WORLD);
    }
    
    // Receive localM
    if (rank == 0)
    {
        // We don't have to send localM for process with rank 0.
        m += localM;

        int receivedM;
        for (i = 1; i < size; i++)
        {
            MPI_Recv(&receivedM, 1, MPI_INT, i, 1991, MPI_COMM_WORLD, NULL);
            //printf("RANK {%d}: Received localM %d.\n", rank, receivedM);

            m += receivedM;
        }
    }

    // Calculate pi
    if (rank == 0)
    {
        double pi = calculatePi(m, APPROXIMATION_LEVEL);
        printf("The pi approximation is: %lf\n", pi);
    }

    /* Finalize */
    MPI_Finalize();

    return 0;
}