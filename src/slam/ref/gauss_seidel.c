#include "slam.h"

static int converge(double* x, double* prev_x, int size, double eps) {
    double norm = 0;
    for (int i = 0; i < size; i++) {
        norm += (x[i] - prev_x[i]) * (x[i] - prev_x[i]);
    }
    return (sqrt(norm) < eps);
}

// size - размерность матрицы
// A - матрица коэффициентов
// b - матрица свободных членов
// х - результат
extern inline uint32_t gauss_seidel(uint32_t size, double* A, double* b, double* x, double eps) {
    double* prev_x = (double*)malloc(size * sizeof(double));
    double norm;
    uint32_t iterations = 0;

    // начальное приближение x = b
    for (int i = 0; i < size; i++) {
        x[i] = b[i];
    }

    do {
        for (int i = 0; i < size; i++) {
            prev_x[i] = x[i];
        }
        for (int i = 0; i < size; i++) {
            double var = 0;
            for (int j = 0; j < size; j++) {
                if (i != j) {
                    var += A[i * size + j] * x[j];
                }
            }
            x[i] = (b[i] - var) / A[i * size + i];
        }
        iterations++;
    } while (!converge(x, prev_x, size, eps));

    free(prev_x);
    return iterations;
}
