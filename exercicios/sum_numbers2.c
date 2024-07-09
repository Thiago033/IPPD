#include <omp.h>
#include <stdio.h>

int main() {
    #pragma omp parallel
    {
        printf("Hello, World! from thread %d\n", omp_get_thread_num());
    }
    return 0;
}



#include <omp.h>
#include <stdio.h>

int main() {
    int N = 100;
    int sum = 0;

    #pragma omp parallel for reduction(+:sum)
    for (int i = 1; i <= N; i++) {
        sum += i;
    }

    printf("Sum of first %d natural numbers is %d\n", N, sum);
    return 0;
}



#include <omp.h>
#include <stdio.h>

int main() {
    int count = 0;

    #pragma omp parallel
    {
        #pragma omp critical
        {
            count++;
        }
    }

    printf("Count is %d\n", count);
    return 0;
}



#include <omp.h>
#include <stdio.h>

int main() {
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            printf("Section 1 executed by thread %d\n", omp_get_thread_num());
        }
        #pragma omp section
        {
            printf("Section 2 executed by thread %d\n", omp_get_thread_num());
        }
    }
    return 0;
}



#include <omp.h>
#include <stdio.h>

int main() {
    #pragma omp parallel
    {
        #pragma omp single
        {
            printf("Single section executed by thread %d\n", omp_get_thread_num());
        }
    }
    return 0;
}



#include <omp.h>
#include <stdio.h>

int main() {
    #pragma omp parallel
    {
        int thread_num = omp_get_thread_num();
        printf("Thread %d before barrier\n", thread_num);

        #pragma omp barrier

        printf("Thread %d after barrier\n", thread_num);
    }
    return 0;
}



#include <omp.h>
#include <stdio.h>

int main() {
    #pragma omp parallel
    {
        #pragma omp single
        {
            #pragma omp task
            {
                printf("Task 1 executed by thread %d\n", omp_get_thread_num());
            }
            #pragma omp task
            {
                printf("Task 2 executed by thread %d\n", omp_get_thread_num());
            }
        }
    }
    return 0;
}



#include <omp.h>
#include <stdio.h>
#define N 100

int main() {
    int A[N][N], B[N][N], C[N][N];

    // Initialize matrices A and B
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = i + j;
            B[i][j] = i - j;
        }
    }

    // Parallelize matrix multiplication
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = 0;
            for (int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    // Print result (optional, for small N)
    printf("Result matrix C[0][0]: %d\n", C[0][0]);
    return 0;
}




#include <omp.h>
#include <stdio.h>

int fib(int n) {
    int x, y;
    if (n < 2) {
        return n;
    } else {
        #pragma omp task shared(x)
        x = fib(n - 1);
        #pragma omp task shared(y)
        y = fib(n - 2);
        #pragma omp taskwait
        return x + y;
    }
}

int main() {
    int n = 10;
    int result;
    
    #pragma omp parallel
    {
        #pragma omp single
        result = fib(n);
    }

    printf("Fibonacci of %d is %d\n", n, result);
    return 0;
}

