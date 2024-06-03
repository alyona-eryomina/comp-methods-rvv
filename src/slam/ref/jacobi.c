#include "slam.h"

// size - размерность матрицы
// A - матрица коэффициентов
// b - матрица свободных членов
// х - результат
extern inline uint32_t jacobi(uint32_t size, double* A, double* b, double* x, double eps, SLAMJacobiBuf* buf) {
    double norm;
    uint32_t iterations = 0;
    // начальное приближение x = b
    for (int i = 0; i < size; i++) {
        x[i] = b[i];
    }

    do {
        for (int i = 0; i < size; i++) {
            buf->tmp_x[i] = b[i];
            for (int j = 0; j < size; j++) {
                if (i != j) {
                    buf->tmp_x[i] -= A[i * size + j] * x[j];
                }
            }
            buf->tmp_x[i] /= A[i * size + i];
        }

        norm = fabs(x[0] - buf->tmp_x[0]);
        for (int i = 0; i < size; i++) {
            if (fabs(x[i] - buf->tmp_x[i]) > norm) {
                norm = fabs(x[i] - buf->tmp_x[i]);
            }
            x[i] = buf->tmp_x[i];
        }
        iterations++;
    } while (norm > eps);

    return iterations;
}
