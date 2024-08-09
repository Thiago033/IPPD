#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <time.h>

#define N 1000

void calculate(int *matrix, int size, int rank, int world_size, double *global_sum, double *global_sum_sq, int *global_zero_count) {
    int local_zero_count = 0;
    double local_sum = 0, local_sum_sq = 0;

    for (int i = 0; i < size; i++) {
        int value = matrix[i];

        if (value == 0) local_zero_count++;

        local_sum += value;
        local_sum_sq += value * value;
    }

    MPI_Reduce(&local_sum, global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_sum_sq, global_sum_sq, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_zero_count, global_zero_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
}

int main(int argc, char *argv[]) {
    int rank, world_size;
    int *matrix = NULL;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int elements_per_process = (N * N) / world_size;
    int *sub_matrix = malloc(elements_per_process * sizeof(int));

    // iniciar matrix
    if (rank == 0) {
        matrix = malloc(N * N * sizeof(int));

        srand(time(NULL));

        for (int i = 0; i < N * N; i++) {
            matrix[i] = rand() % 101;
        }
    }

    MPI_Scatter(matrix, elements_per_process, MPI_INT, sub_matrix, elements_per_process, MPI_INT, 0, MPI_COMM_WORLD);

    int global_zero_count;
    double global_sum, global_sum_sq;

    calculate(sub_matrix, elements_per_process, rank, world_size, &global_sum, &global_sum_sq, &global_zero_count);

    if (rank == 0) {
        double mean = global_sum / (N * N);
        double variance = (global_sum_sq / (N * N)) - (mean * mean);
        double stddev = sqrt(variance);
        int sparse_threshold = (int)(0.05 * N * N);

        printf("Media: %f\n", mean);
        printf("Desvio padrao: %f\n", stddev);
        printf("Numero de zeros: %d\n", global_zero_count);
        printf("A matriz %s eh esparsa.\n", global_zero_count > sparse_threshold ? "" : "nao ");
        
        free(matrix);
    }

    free(sub_matrix);

    MPI_Finalize();

    return 0;
}
