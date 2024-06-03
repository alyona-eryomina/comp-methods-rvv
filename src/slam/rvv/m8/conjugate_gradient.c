#include "slam.h"

// size - размерность матрицы
// A - матрица коэффициентов
// b - матрица свободных членов
// х - результат
extern inline uint32_t conjugate_gradient_rvv(uint32_t size, double* A, double* b, double* x, double eps, SLAMGradBuf* buf) {
    uint32_t iterations = 0;

    vfloat64m8_t vX, vR, vZ, vS;
    vfloat64m1_t vSpr0, vSpr1, vSpz, tmp;

    vSpr0 = vfmv_v_f_f64m1(0.0, 1);
    double spr_0 = 0.0;
    // Начальное приближение
    size_t len = size;
    size_t offset = 0;
    for (;(len > 0);) {
        size_t l = vsetvl_e64m8(len);

        vX = vfmv_v_f_f64m8(0.0, l);
        vse64_v_f64m8(x + offset, vX, l);

        vR = vle64_v_f64m8(b + offset, l);
        vse64_v_f64m8(buf->r + offset, vR, l);
        vse64_v_f64m8(buf->z + offset, vR, l);
        
        vR = vfmul_vv_f64m8(vR, vR, l);
        vSpr0 = vfredosum_vs_f64m8_f64m1(vSpr0, vR, vSpr0, l);

        len -= l;
        offset += l;
    }
    vse64_v_f64m1(&spr_0, vSpr0, 1);

    double spz = 0.0;
    double spr_1 = 0.0;
    double beta = 0.0;
    do {
        // Вычисляем числитель и знаменатель для коэффициента
        // alpha = (rk-1,rk-1)/(Azk-1,zk-1) */
        for (int i = 0; i < size; i++) {
            tmp = vfmv_v_f_f64m1(0.0, 1);

            len = size;
            offset = 0;
            for (;(len > 0);) {
                size_t l = vsetvl_e64m8(len);

                vR = vle64_v_f64m8(A + i * size + offset, l);
                vZ = vle64_v_f64m8(buf->z + offset, l);

                vR = vfmul_vv_f64m8(vR, vZ, l);
                tmp = vfredosum_vs_f64m8_f64m1(tmp, vR, tmp, l);

                len -= l;
                offset += l;
            }
            vse64_v_f64m1(buf->s + i, tmp, 1);
        }

        vSpz = vfmv_v_f_f64m1(0.0, 1);
        len = size;
        offset = 0;
        for (;(len > 0);) {
            size_t l = vsetvl_e64m8(len);

            vS = vle64_v_f64m8(buf->s + offset, l);
            vZ = vle64_v_f64m8(buf->z + offset, l);

            vS = vfmul_vv_f64m8(vS, vZ, l);
            vSpz = vfredosum_vs_f64m8_f64m1(vSpz, vS, vSpz, l);

            len -= l;
            offset += l;
        }
        vse64_v_f64m1(&spz, vSpz, 1);

        double alpha = spr_0 / spz;

        // Вычисляем вектор решения: xk = xk-1+ alpha * zk-1, 
        // вектор невязки: rk = rk-1 - alpha * A * zk-1 и числитель для betaa равный (rk,rk)
        vSpr1 = vfmv_v_f_f64m1(0.0, 1);
        len = size;
        offset = 0;
        for (;(len > 0);) {
            size_t l = vsetvl_e64m8(len);

            vX = vle64_v_f64m8(x + offset, l);
            vZ = vle64_v_f64m8(buf->z + offset, l);
            vX = vfmacc_vf_f64m8(vX, alpha, vZ, l);
            vse64_v_f64m8(x + offset, vX, l);
            
            vR = vle64_v_f64m8(buf->r + offset, l);
            vS = vle64_v_f64m8(buf->s + offset, l);
            vR = vfnmsac_vf_f64m8(vR, alpha, vS, l);
            vse64_v_f64m8(buf->r + offset, vR, l);

            vR = vfmul_vv_f64m8(vR, vR, l);
            vSpr1 = vfredosum_vs_f64m8_f64m1(vSpr1, vR, vSpr1, l);

            len -= l;
            offset += l;
        }
        vse64_v_f64m1(&spr_1, vSpr1, 1);

        // вычисляем beta
        beta = spr_1 / spr_0;

        // Вычисляем вектор спуска: zk = rk+ beta * zk-1
        len = size;
        offset = 0;
        for (;(len > 0);) {
            size_t l = vsetvl_e64m8(len);

            vZ = vle64_v_f64m8(buf->z + offset, l);
            vR = vle64_v_f64m8(buf->r + offset, l);
            vZ = vfmadd_vf_f64m8(vZ, beta, vR, l);
            vse64_v_f64m8(buf->z + offset, vZ, l);

            len -= l;
            offset += l;
        }

        spr_0 = spr_1;

        iterations += 1;
        //printf("%d: %f\n", iterations, sqrt(spr_0));
    } while (sqrt(spr_0) > eps);

    return iterations;
}
