#include <math.h>
#include <stdlib.h>
#include <stdint.h>

// size - размерность матрицы
// A - матрица коэффициентов
// b - матрица свободных членов
// х - результат
extern inline uint32_t jacobi(uint32_t size, double* A, double* b, double* x, double eps) {
    double* tmp_x = (double*)malloc(size * sizeof(double));
    double norm;
    uint32_t iterations = 0;
    // начальное приближение x = b
    for (int i = 0; i < size; i++) {
        x[i] = b[i];
    }

    do {
        for (int i = 0; i < size; i++) {
            tmp_x[i] = b[i];
            for (int j = 0; j < size; j++) {
                if (i != j) {
                    tmp_x[i] -= A[i * size + j] * x[j];
                }
            }
            tmp_x[i] /= A[i * size + i];
        }

        norm = fabs(x[0] - tmp_x[0]);
        for (int i = 0; i < size; i++) {
            if (fabs(x[i] - tmp_x[i]) > norm) {
                norm = fabs(x[i] - tmp_x[i]);
            }
            x[i] = tmp_x[i];
        }
        iterations++;
    } while (norm > eps);

    free(tmp_x);
    return iterations;
}