#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define N 1000

void calculate_statistics(int *matrix, int size, int rank, int world_size, int *global_neg_count, int *global_max, int *global_min, double *global_sum, double *global_sum_sq, int *global_zero_count) {
    int local_neg_count = 0, local_max = matrix[0], local_min = matrix[0], local_zero_count = 0;
    double local_sum = 0, local_sum_sq = 0;

    for (int i = 0; i < size; i++) {
        int value = matrix[i];
        if (value < 0) local_neg_count++;
        if (value > local_max) local_max = value;
        if (value < local_min) local_min = value;
        if (value == 0) local_zero_count++;

        local_sum += value;
        local_sum_sq += value * value;
    }

    MPI_Reduce(&local_neg_count, global_neg_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_max, global_max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_min, global_min, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
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

    if (rank == 0) {
        matrix = malloc(N * N * sizeof(int));
        srand(time(NULL));
        for (int i = 0; i < N * N; i++) {
            matrix[i] = rand() % 201 - 100; // Valores entre -100 e 100
        }
    }

    MPI_Scatter(matrix, elements_per_process, MPI_INT, sub_matrix, elements_per_process, MPI_INT, 0, MPI_COMM_WORLD);

    int global_neg_count, global_max, global_min, global_zero_count;
    double global_sum, global_sum_sq;

    calculate_statistics(sub_matrix, elements_per_process, rank, world_size, &global_neg_count, &global_max, &global_min, &global_sum, &global_sum_sq, &global_zero_count);

    if (rank == 0) {
        double mean = global_sum / (N * N);
        double variance = (global_sum_sq / (N * N)) - (mean * mean);
        double stddev = sqrt(variance);
        int sparse_threshold = (int)(0.05 * N * N);

        printf("Número de elementos negativos: %d\n", global_neg_count);
        printf("Maior valor: %d\n", global_max);
        printf("Menor valor: %d\n", global_min);
        printf("Média: %f\n", mean);
        printf("Desvio padrão: %f\n", stddev);
        printf("Número de zeros: %d\n", global_zero_count);
        printf("A matriz %sé esparsa.\n", global_zero_count > sparse_threshold ? "" : "não ");
        
        free(matrix);
    }

    free(sub_matrix);
    MPI_Finalize();
    return 0;
}
