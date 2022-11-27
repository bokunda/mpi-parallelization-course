/********************************************************
Example 04: PI approximation using Monte Carlo method

PI approximation is done using a Monte Carrlo method.
m = amount of dots with condtion xi^2 * yi^2 <= 1;
ro = m / n;
Pi = 4 * ro;

Idea is to parallize calculation of m: xi^2 * yi^2 <= 1
and to send localM values to one node.

Example covers:
- MPI_Send()
- MPI_Recv()
- MPI_Bcast()
- MPI_Reduce()
*********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

int satisfiesGraphicCondition(double x, double y)
{
    return x*x + y*y <= 1;
}

int calculateLocalM(int size, int approximationLevel)
{
    int i, localM = 0;
    double x, y;
    for (i = 0; i < approximationLevel / size; i++)
    {
        // rand() returns int in interval 0-RAND_MAX
        x = (double)rand() / RAND_MAX;
        y = (double)rand() / RAND_MAX;

        localM += satisfiesGraphicCondition(x, y);
    }
    return localM;
}

double calculateRo(int m, int n)
{
    return (double)m / n;
}

double calculatePi(int m, int n)
{
    return 4 * calculateRo(m, n);
}

void doWork_send_receive(int localM, long approximationLevel, int rank, int size)
{
    int i = 0, m = 0;
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
        double pi = calculatePi(m, approximationLevel);
        printf("The pi approximation is: %lf [MPI_Send(), MPI_Recv()]\n", pi);
    }
}

void doWork_mpi_reduce(int localM, int approximationLevel, int rank, int size)
{
    int m;
    MPI_Reduce(&localM, &m, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        double pi = calculatePi(m, approximationLevel);
        printf("The pi approximation is: %lf [MPI_Reduce()]\n", pi);
    }
}

int main(int argc, char **argv)
{
    /* Init */
    MPI_Init(&argc, &argv);

    /* Main part */
    int size, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    srand(rank);

    long approximationLevel;
    if (rank == 0)
    {
        printf("Enter approximation level: \n");
        scanf("%ld", &approximationLevel);
    }
    MPI_Bcast(&approximationLevel, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calculate local m
    // Parallelization part => we have to calculate
    // APPROXIMATION_LEVEL amount of dots, so we can 
    // split that into APPROXIMATION_LEVEL / size batches
    // and calculate a local sum of m.
    int localM = calculateLocalM(size, approximationLevel);

    // Example 1: Use MPI_Send() and MPI_Recv() for communication.
    doWork_send_receive(localM, approximationLevel, rank, size);

    // Example 2: Use MPI_Reduce() for communication.
    doWork_mpi_reduce(localM, approximationLevel, rank, size);

    /* Finalize */
    MPI_Finalize();

    return 0;
}