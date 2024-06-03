#include "slam.h"

static int converge(double* x, double* prev_x, int size, double eps) {
    vfloat64m1_t vNorm;
    vfloat64m2_t vX, vP;
    double norm;
    
    vNorm = vfmv_v_f_f64m1(0.0, 1);
    
    size_t len = size;
    size_t offset = 0;
    for (;(len > 0);) {
        size_t l = vsetvl_e64m2(len);

        vX = vle64_v_f64m2(x + offset, l);
        vP = vle64_v_f64m2(prev_x + offset, l);
        vX = vfsub_vv_f64m2(vX, vP, l);
        vX = vfmul_vv_f64m2(vX, vX, l);
        vNorm = vfredosum_vs_f64m2_f64m1(vNorm, vX, vNorm, l);

        len -= l;
        offset += l;
    }

    vse64_v_f64m1(&norm, vNorm, 1);
    return (sqrt(norm) < eps);
}

// size - размерность матрицы
// A - матрица коэффициентов
// b - матрица свободных членов
// х - результат
extern inline uint32_t gauss_seidel_rvv(uint32_t size, double* A, double* b, double* x, double eps, SLAMGaussBuf* buf) {
    double norm;
    uint32_t iterations = 0;
    vfloat64m2_t vA, vX;
    vfloat64m1_t tmp;

    // начальное приближение x = b
    size_t len = size;
    size_t offset = 0;
    for (;(len > 0);) {
        size_t l = vsetvl_e64m2(len);
        vX = vle64_v_f64m2(b + offset, l);
        vse64_v_f64m2(x + offset, vX, l);

        len -= l;
        offset += l;
    }

    do {
        len = size;
        offset = 0;
        for (;(len > 0);) {
            size_t l = vsetvl_e64m2(len);
            vX = vle64_v_f64m2(x + offset, l);
            vse64_v_f64m2(buf->tmp_x + offset, vX, l);

            len -= l;
            offset += l;
        }

        for (int i = 0; i < size; i++) {
            tmp = vfmv_v_f_f64m1(-A[i * size + i] * x[i], 1);
            len = size;
            offset = 0;
            for (;(len > 0);) {
                size_t l = vsetvl_e64m2(len);
    
                vX = vle64_v_f64m2(x + offset, l);
                vA = vle64_v_f64m2(A + i * size + offset, l);
                vX = vfmul_vv_f64m2(vA, vX, l);
                tmp = vfredosum_vs_f64m2_f64m1(tmp, vX, tmp, l);
    
                len -= l;
                offset += l;
            }
            double var;
            vse64_v_f64m1(&var, tmp, 1);

            x[i] = (b[i] - var) / A[i * size + i];
        }
        iterations++;
    } while (!converge(x, buf->tmp_x, size, eps));

    return iterations;
}
