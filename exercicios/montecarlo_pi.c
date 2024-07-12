#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[]) {
    int num_tasks, rank, i, count = 0, total_count;
    long num_points, points_per_task;
    double x, y, z, pi;

    // inicializa o MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (argc != 2) {
        // no processo raiz (rank 0)
        if (rank == 0) {
            printf("Usage: %s <number_of_points>\n", argv[0]);
        }
        MPI_Finalize();
        return -1;
    }

    num_points = atol(argv[1]);
    points_per_task = num_points / num_tasks;

    srand(time(NULL) + rank);

    // geração de pontos aleatorios e contagem de pontos
    for (i = 0; i < points_per_task; i++) {
        x = (double)rand() / RAND_MAX;
        y = (double)rand() / RAND_MAX;
        z = x * x + y * y;
        if (z <= 1.0) {
            count++;
        }
    }

    MPI_Reduce(&count, &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // no processo raiz (rank 0)
    if (rank == 0) {
        pi = ((double)total_count / (double)num_points) * 4.0;
        printf("Aproximacao de Pi: %f\n", pi);
    }

    MPI_Finalize();
    return 0;
}
