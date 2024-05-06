#include "comp_methods.h"
#include "slam.h"

// Требования:
// 1. Матрица size x size
// 2. Диагональное преобладание: SLAM_JACOBI, SLAM_GAUSS_SEIDEL

uint32_t slam(uint32_t size, double* A, double* b, double* x, double eps, SlamMethod method) {
    uint32_t iterations;
    switch (method) {
    case SLAM_JACOBI:
        iterations = jacobi(size, A, b, x, eps);
        break;
    case SLAM_GAUSS_SEIDEL:
        iterations = gauss_seidel(size, A, b, x, eps);
        break;
    default:
        break;
    }
    return iterations;
}
