#include <stdio.h>
#include <omp.h>

#define NUM_THREADS 2

static long num_steps = 1000;
double step;

void main() {
    omp_set_num_threads(NUM_THREADS);
    double pi;
    int nthreads= NUM_THREADS;

    step = 1.0/(double)num_steps;

    #pragma omp parallel
    {
        int i, id, nthreads2;
        double x, sum;

        id = omp_get_thread_num();

        nthreads2 = omp_get_num_threads();

        if ( id == 0 ) nthreads = nthreads2;
  
        for (i = id, sum = 0.0; i < num_steps; i = i + nthreads) {
            x = (i + 0.5) * step;
            sum += 4.0/(1.0+x*x);
        }

        #pragma omp critical
            pi += sum * step;
    }

    printf("%f", pi);
}
