// mpicc -o soma_vetores soma_vetores.c
// mpirun -np 4 ./soma_vetores

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    int size, rank;
    int n = 100;
    int *A = NULL, *B = NULL, *C = NULL;
    int *sub_A, *sub_B, *sub_C;
    int local_n;

    // inicializa o MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // tamanho da submatriz que cada processo vai manipular
    local_n = n / size;

    sub_A = (int*)malloc(local_n * sizeof(int));
    sub_B = (int*)malloc(local_n * sizeof(int));
    sub_C = (int*)malloc(local_n * sizeof(int));

    // no processo raiz (rank 0)
    if (rank == 0) {
        A = (int*)malloc(n * sizeof(int));
        B = (int*)malloc(n * sizeof(int));
        C = (int*)malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) {
            A[i] = rand() % 100;
            B[i] = rand() % 100;
        }
    }

    // distribui partes dos vetores A e B para todas as tasks
    MPI_Scatter(A, local_n, MPI_INT, sub_A, local_n, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(B, local_n, MPI_INT, sub_B, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    // cada task faz a soma dos elementos dos vetores A e B
    for (int i = 0; i < local_n; i++) {
        sub_C[i] = sub_A[i] + sub_B[i];
    }

    // junta os vetores C locais em um vetor C global no processo raiz
    MPI_Gather(sub_C, local_n, MPI_INT, C, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    // no processo raiz (rank 0)
    if (rank == 0) {
        printf("Vetor C:\n");
        for (int i = 0; i < n; i++) {
            printf("%d ", C[i]);
        }
        printf("\n");

        free(A);
        free(B);
        free(C);
    }

    free(sub_A);
    free(sub_B);
    free(sub_C);

    MPI_Finalize();

    return 0;
}
