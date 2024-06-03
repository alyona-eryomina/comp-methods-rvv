#include "slam.h"

extern inline uint32_t jacobi_rvv(uint32_t size, double* A, double* b, double* x, double eps, SLAMJacobiBuf* buf) {
    double norm;
    uint32_t iterations = 0;
    vfloat64m8_t vTmpX, vA, vX, vAX, vNorm;
    vfloat64m1_t tmp, norm1;
    vuint64m8_t vInd;

    // начальное приближение x = b
    size_t len = size;
    size_t offset = 0;
    for (;(len > 0);) {
        size_t l = vsetvl_e64m8(len);
        vX = vle64_v_f64m8(b + offset, l);
        vse64_v_f64m8(x + offset, vX, l);

        len -= l;
        offset += l;
    }

    // индексы диагональных элементов
    for (int i = 0; i < size; i++) {
        buf->index[i] = i * size + i;
    }
    
    do {
        // dotprod по строкам
        for (int i = 0; i < size; i++) {
            tmp = vfmv_v_f_f64m1(0.0, 1);
            len = size;
            offset = 0;
            for (;(len > 0);) {
                size_t l = vsetvl_e64m8(len);
    
                vX = vle64_v_f64m8(x + offset, l);
                vA = vle64_v_f64m8(A + i * size + offset, l);
                vX = vfmul_vv_f64m8(vA, vX, l);
                tmp = vfredosum_vs_f64m8_f64m1(tmp, vX, tmp, l);
    
                len -= l;
                offset += l;
            }
            vse64_v_f64m1(buf->tmp_x + i, tmp, 1);
        }
    
        len = size;
        offset = 0;
        
        norm1 = vfmv_v_f_f64m1(0.0, 1);
        for (;(len > 0);) {
            size_t l = vsetvl_e64m8(len);
            vTmpX = vle64_v_f64m8(b + offset, l);
            vAX = vle64_v_f64m8(buf->tmp_x + offset, l);
    
            vInd = vle64_v_u64m8(buf->index + offset, l);
            vInd = vmul_vx_u64m8(vInd, sizeof(double), l);
            vA = vloxei64_v_f64m8(A, vInd, l);
            vX = vle64_v_f64m8(x + offset, l);
            
            vAX = vfmsub_vv_f64m8(vA, vX, vAX, l);
            vTmpX = vfadd_vv_f64m8(vTmpX, vAX, l);
            vTmpX = vfdiv_vv_f64m8(vTmpX, vA, l);
            vse64_v_f64m8(x + offset, vTmpX, l);

            vNorm = vfsub_vv_f64m8(vX, vTmpX, l);
            vNorm = vfabs_v_f64m8(vNorm, l);
            norm1 = vfredmax_vs_f64m8_f64m1(norm1, vNorm, norm1, l);

            len -= l;
            offset += l;
        }

        vse64_v_f64m1(&norm, norm1, 1);
        iterations++;
    
    } while (norm > eps);

    return iterations;
}
