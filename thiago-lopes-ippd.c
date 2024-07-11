// Comando para compilaçao:
// gcc -fopenmp thiago-lopes-ippd.c -o thiago-lopes-ippd -lm

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <math.h>

#define N 10

int main() {
    double start_time, end_time;

    start_time = omp_get_wtime();

    srand(time(NULL));

    int A[N][N];

    // Inicializa a matriz A
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = rand() % 100; // entre 0 e 100
        }
    }

    // //Test
    // A[0][0] = 1;
    // A[0][1] = 0;
    // A[1][0] = 0;
    // A[1][1] = 0;

    // for (int i = 0; i < N; i++) {
    //     for (int j = 0; j < N; j++) {
    //         printf("%d |" , A[i][j]);
    //     }
    //     printf("\n");
    // }

    double sum = 0;

    // Soma de todos os elementos da matriz
    #pragma omp parallel for collapse(2) reduction(+:sum)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            sum += A[i][j];
        }
    }

    int matrix_total_elements = N * N;
    double media = sum / matrix_total_elements;

    // Desvio padrao
    double p = 0;
    #pragma omp parallel for collapse(2) reduction(+:p)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            p += pow(A[i][j] - media, 2);
        }
    }
    double desvio_padrao = sqrt(p / matrix_total_elements);

    // Matriz esparsa
    int count = 0;
    #pragma omp parallel for collapse(2) reduction(+:count)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (A[i][j] == 0) {
                count++;
            }
        }
    }

    printf("Media: %f \n", media);
    printf("Desvio Padrao: %f \n", desvio_padrao);

    printf("Numero de valores nulos na matriz: %d\n", count);
    if (count >= (matrix_total_elements / 2)) {
        printf("Matriz é esparsa\n");
    } else {
        printf("Matriz nao é esparsa\n");
    }

    end_time = omp_get_wtime();

    printf("Tempo de execucao: %f segundos\n", end_time - start_time);

    return 0;
}