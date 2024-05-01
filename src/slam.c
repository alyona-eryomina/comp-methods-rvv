#include "comp_methods.h"

#ifdef RVV07
extern inline uint32_t jacobi_rvv(uint32_t size, double* A, double* b, double* x, double eps);
extern inline uint32_t gauss_seidel(uint32_t size, double* A, double* b, double* x, double eps);
#elif defined RVV
extern inline uint32_t jacobi(uint32_t size, double* A, double* b, double* x, double eps);
extern inline uint32_t gauss_seidel(uint32_t size, double* A, double* b, double* x, double eps);
#else
extern inline uint32_t jacobi(uint32_t size, double* A, double* b, double* x, double eps);
extern inline uint32_t gauss_seidel(uint32_t size, double* A, double* b, double* x, double eps);
#endif

// Требования:
// 1. Матрица size x size
// 2. Диагональное преобладание: SLAM_JACOBI, SLAM_GAUSS_SEIDEL

uint32_t slam(uint32_t size, double* A, double* b, double* x, double eps, SlamMethod method) {
    uint32_t iterations;
    switch (method) {
    case SLAM_JACOBI:
#if defined(RVV07) || defined(RVV_ASM)
        iterations = jacobi_rvv(size, A, b, x, eps);
#else
        iterations = jacobi(size, A, b, x, eps);
#endif
        break;
    case SLAM_GAUSS_SEIDEL:
        iterations = gauss_seidel(size, A, b, x, eps);
        break;
    default:
        break;
    }
    return iterations;
}
