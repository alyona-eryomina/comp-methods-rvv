#include "slam.h"

// extern inline uint32_t jacobi_rvv(uint32_t size, double* A, double* b, double* x, double eps) {
//     double* tmp_x = (double*)malloc(size * sizeof(double));
//     double norm;
//     uint32_t iterations = 0;
//     // начальное приближение x = b
//     for (int i = 0; i < size; i++) {
//         x[i] = b[i];
//     }
//
//     do {
//         for (int i = 0; i < size; i++) {
//             tmp_x[i] = b[i];
//             for (int j = 0; j < size; j++) {
//                 if (i != j) {
//                     tmp_x[i] -= A[i * size + j] * x[j];
//                 }
//             }
//             tmp_x[i] /= A[i * size + i];
//         }
//
//         norm = fabs(x[0] - tmp_x[0]);
//         for (int i = 0; i < size; i++) {
//             if (fabs(x[i] - tmp_x[i]) > norm) {
//                 norm = fabs(x[i] - tmp_x[i]);
//             }
//             x[i] = tmp_x[i];
//         }
//         iterations++;
//     } while (norm > eps);
//
//     free(tmp_x);
//     return iterations;
// }

extern inline uint32_t jacobi_rvv(uint32_t size, double* A, double* b, double* x, double eps) {
    double* tmp_x = (double*)malloc(size * sizeof(double));
    double norm;
    uint32_t iterations = 0;
    // начальное приближение x = b
    for (int i = 0; i < size; i++) {
        x[i] = b[i];
    }

    // индексы диагональных элементов
    uint64_t *ind = (uint64_t*)malloc(size * sizeof(uint64_t));
    for (int i = 0; i < size; i++) {
        ind[i] = i * size + i;
    }

    double* ax = (double*)malloc(size * sizeof(double));

    do {
        vfloat64m8_t vTmpX, vA, vX;
        vuint64m8_t vInd;

        // dotprod по строкам
        for (int i = 0; i < size; i++) {
            vfloat64m1_t ttt = vfmv_v_f_f64m1(-A[i * size + i] * x[i], 1);
            size_t lenY = size;
            size_t offsetY = 0;
            for (;(lenY > 0);) {
                size_t ly = vsetvl_e64m8(lenY);

                vX = vle64_v_f64m8(x + offsetY, ly);
                vA = vle64_v_f64m8(A + i * size + offsetY, ly);
                vfloat64m8_t tt = vfmul_vv_f64m8(vA, vX, ly);
                ttt = vfredosum_vs_f64m8_f64m1(ttt, tt, ttt, ly);

                lenY -= ly;
                offsetY += ly;
            }
            vse64_v_f64m1(ax + i, ttt, 1);
        }

        size_t lenX = size;
        size_t offsetX = 0;
        for (;(lenX > 0);) {
            size_t lx = vsetvl_e64m8(lenX);
            vTmpX = vle64_v_f64m8(b + offsetX, lx);
            vA = vle64_v_f64m8(ax + offsetX, lx);
            vTmpX = vfsub_vv_f64m8(vTmpX, vA, lx);

            vInd = vle64_v_u64m8(ind + offsetX, lx);
            vInd = vmul_vx_u64m8(vInd, sizeof(double), lx);
            vA = vloxei64_v_f64m8(A, vInd, lx);
            vTmpX = vfdiv_vv_f64m8(vTmpX, vA, lx);
            vse64_v_f64m8(tmp_x + offsetX, vTmpX, lx);

            lenX -= lx;
            offsetX += lx;
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

    // do {
    //     size_t lenX = size;
    //     size_t offsetX = 0;

    //     vfloat64m8_t vTmpX, vA, vX, vSub;
    //     vuint64m8_t vInd;

    //     for (int i = 0; i < size; i++) {
    //         tmp_x[i] = b[i];

    //         // vfloat64m1_t ttt = vfmv_v_f_f64m1(-A[i * size + i] * x[i], 1);
    //         // size_t lenY = size;
    //         // size_t offsetY = 0;
    //         // for (;(lenY > 0);) {
    //         //     size_t ly = vsetvl_e64m8(lenY);

    //         //     vX = vle64_v_f64m8(x + offsetY, ly);
    //         //     vA = vle64_v_f64m8(A + i * size + offsetY, ly);
    //         //     vfloat64m8_t tt = vfmul_vv_f64m8(vA, vX, ly);
    //         //     ttt = vfredosum_vs_f64m8_f64m1(ttt, tt, ttt, ly);

    //         //     lenY -= ly;
    //         //     offsetY += ly;
    //         // }
    //         // double tttt;
    //         // vse64_v_f64m1(&tttt, ttt, 1);
    //         // tmp_x[i] -= tttt;
    //         // tmp_x[i] /= A[i * size + i];
    //         for (int j = 0; j < size; j++) {
    //             if (i != j) {
    //                 tmp_x[i] -= A[i * size + j] * x[j];
    //             }
    //         }
    //         tmp_x[i] /= A[i * size + i];
    //     }

    //     norm = fabs(x[0] - tmp_x[0]);
    //     for (int i = 0; i < size; i++) {
    //         if (fabs(x[i] - tmp_x[i]) > norm) {
    //             norm = fabs(x[i] - tmp_x[i]);
    //         }
    //         x[i] = tmp_x[i];
    //     }
    //     iterations++;
    // } while (norm > eps);

    free(tmp_x);
    free(ind);
    free(ax);
    return iterations;
}
