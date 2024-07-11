#include <stdio.h>
#include <omp.h>

int main() {
    int sum = 0;

    omp_set_num_threads(4);

    #pragma omp parallel for reduction(+:sum)
    for (int i = 1; i <= 100; i++) {
        sum += i;
    }

    printf("Sum of numbers from 1 to 100 is: %d\n", sum);
    return 0;
}
