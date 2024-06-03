#include "comp_methods.h"
#include "slam.h"

// Требования:
// 1. Матрица size x size
// 2. Диагональное преобладание: SLAM_JACOBI, SLAM_GAUSS_SEIDEL

uint32_t slam(uint32_t size, double* A, double* b, double* x, double eps, SlamMethod method) {
    uint32_t iterations;
    SLAMJacobiBuf jBuf;
    SLAMGaussBuf gBuf;
    SLAMGradBuf grBuf;
    switch (method) {
    case SLAM_JACOBI:
        init_jacobi_buf(&jBuf, size);
        iterations = jacobi(size, A, b, x, eps, &jBuf);
        // printf("eps: %lf\n", eps);
        // printf("A: ");
        // for (int i = 0; i < size*size; i++) {
        //     printf("%lf ", A[i]);
        // }
        // printf("\nb: ");
        // for (int i = 0; i < size; i++) {
        //     printf("%lf ", b[i]);
        // }
        // printf("\nx: ");
        // for (int i = 0; i < size; i++) {
        //     printf("%lf ", x[i]);
        // }
        // printf("\ntmp: ");
        // for (int i = 0; i < size; i++) {
        //     printf("%lf ", jBuf.tmp_x[i]);
        // }
        // printf("\nindex: ");
        // for (int i = 0; i < size; i++) {
        //     printf("%ld ", jBuf.index[i]);
        // }
        // printf("\n");
        free_jacobi_buf(&jBuf);
        break;
    case SLAM_GAUSS_SEIDEL:
        init_gauss_buf(&gBuf, size);
        iterations = gauss_seidel(size, A, b, x, eps, &gBuf);
        free_gauss_buf(&gBuf);
        break;
    case SLAM_CONJ_GRADIEND:
        init_grad_buf(&grBuf, size);
        iterations = conjugate_gradient(size, A, b, x, eps, &grBuf);
        free_grad_buf(&grBuf);
        break;
    default:
        break;
    }
    return iterations;
}
