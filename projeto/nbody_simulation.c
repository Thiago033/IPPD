#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <omp.h>

#define N 100  // Número de corpos
#define TIMESTEP 0.01  // Passo de tempo
#define G 6.67430e-11  // Constante gravitacional

typedef struct {
    double x, y, z;
    double vx, vy, vz;
    double mass;
} Body;

void update_bodies(Body *bodies, int num_bodies, double dt) {
    #pragma omp parallel for
    for (int i = 0; i < num_bodies; i++) {
        for (int j = 0; j < num_bodies; j++) {
            if (i != j) {
                double dx = bodies[j].x - bodies[i].x;
                double dy = bodies[j].y - bodies[i].y;
                double dz = bodies[j].z - bodies[i].z;
                double dist = sqrt(dx * dx + dy * dy + dz * dz);
                if (dist > 0) {  // Evita divisão por zero
                    double force = (G * bodies[i].mass * bodies[j].mass) / (dist * dist);
                    bodies[i].vx += force * dx / bodies[i].mass * dt;
                    bodies[i].vy += force * dy / bodies[i].mass * dt;
                    bodies[i].vz += force * dz / bodies[i].mass * dt;
                }
            }
        }
        bodies[i].x += bodies[i].vx * dt;
        bodies[i].y += bodies[i].vy * dt;
        bodies[i].z += bodies[i].vz * dt;
    }
}

int main(int argc, char *argv[]) {
    int rank, size;
    Body *bodies = NULL;
    Body *local_bodies = NULL;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        bodies = (Body *)malloc(N * sizeof(Body));
        // Inicialização dos corpos
        for (int i = 0; i < N; i++) {
            bodies[i].x = rand() % 100;
            bodies[i].y = rand() % 100;
            bodies[i].z = rand() % 100;
            bodies[i].vx = 0;
            bodies[i].vy = 0;
            bodies[i].vz = 0;
            bodies[i].mass = rand() % 10 + 1;
        }
    }

    // Distribuição dos dados
    if (rank != 0) {
        bodies = (Body *)malloc(N * sizeof(Body));
    }

    MPI_Bcast(bodies, N * sizeof(Body), MPI_BYTE, 0, MPI_COMM_WORLD);

    double start_time = MPI_Wtime();

    // Simulação de passos
    for (int step = 0; step < 100; step++) {
        update_bodies(bodies, N, TIMESTEP);
    }

    double end_time = MPI_Wtime();

    if (rank == 0) {
        printf("Simulação concluída em %f segundos\n", end_time - start_time);
        free(bodies);
    } else {
        free(bodies);
    }

    MPI_Finalize();
    return 0;
}
