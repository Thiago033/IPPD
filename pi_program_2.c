#include <stdio.h>
#include <omp.h>

static long num_steps = 100000;
double step;

void main() {
    int i;
    double x, pi, sum = 0.0;

    step = 1.0/(double)num_steps;

    #pragma omp parallel
    {
        #pragma omp for private(x) reduction(+:sum)
        for (i = 0; i < num_steps; i++) {
            x = (i + 0.5) * step;
            sum += 4.0/(1.0+x*x);
        }
    }

    pi = sum * step;

    printf("%f", pi);
}
