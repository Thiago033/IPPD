#include <stdio.h>
#include <omp.h>

int main() {
    int sum = 0;
    double start_time, end_time;

    omp_set_num_threads(4);

    start_time = omp_get_wtime(); // Start time measurement

    #pragma omp parallel for reduction(+:sum)
    for (int i = 1; i <= 1000000000; i++) {
        sum += i;
    }
    
    end_time = omp_get_wtime(); // End time measurement

    printf("Sum of numbers from 1 to 100 is: %d\n", sum);
    printf("Time taken: %f seconds\n", end_time - start_time);

    return 0;
}
