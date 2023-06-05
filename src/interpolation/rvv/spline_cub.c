#include "comp_methods_rvv.h"
#include <stdio.h>

extern void SplineCub(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength)
{
    float* a = (float*)malloc(sizeof(float) * (pointLength));
    float* b = (float*)malloc(sizeof(float) * (pointLength));
    float* c = (float*)malloc(sizeof(float) * (pointLength));

    float h = pointX[1] - pointX[0];

    size_t len = pointLength - 1;
    size_t offset = 0;

    vfloat32m8_t vA, vB, vC, vF, vX, vX2, vPointX, vPointY, vPointY2, vPointY3;
    vfloat32m8_t vSrc, vSrc2, vDst, vTmp;
    vuint32m8_t vNum;

    uint32_t index;

    // compute b
    float* va = (float*)malloc(sizeof(float) * (pointLength));
    float* vb = (float*)malloc(sizeof(float) * (pointLength));
    float* vc = (float*)malloc(sizeof(float) * (pointLength));
    float* F  = (float*)malloc(sizeof(float) * (pointLength));
    float coef = 3/(h*h);

    len = pointLength - 1;
    offset = 1;
    for (;(len > 1);)
    {
        size_t l = vsetvl_e32m8(len);
        printf("l = %d\n", l);
        vA = vfmv_v_f_f32m8(1.0f, l);
        vB = vfmv_v_f_f32m8(4.0f, l);
        vC = vfmv_v_f_f32m8(1.0f, l);

        vPointY = vle32_v_f32m8(pointY + offset - 1, l);
        vPointY2 = vle32_v_f32m8(pointY + offset, l);
        vPointY3 = vle32_v_f32m8(pointY + offset + 1, l);
        vPointY2 = vfmul_vf_f32m8(vPointY2, 2.0f, l);
        vF = vfsub_vv_f32m8(vPointY, vPointY2, l);
        vF = vfadd_vv_f32m8(vF, vPointY3, l);
        vF = vfmul_vf_f32m8(vF, coef, l);

        vse32_v_f32m8(va + offset, vA, l);
        vse32_v_f32m8(vb + offset, vB, l);
        vse32_v_f32m8(vc + offset, vC, l);
        vse32_v_f32m8(F + offset, vF, l);

        len -= l;
        offset += l;
    }
    va[0] = 0.0f; vb[0] = 1.0f; vc[0] = 0.0f; F[0] = 0.0f;
    va[pointLength - 1] = 0.0f;
    vb[pointLength - 1] = 1.0f;
    vc[pointLength - 1] = 0.0f;
    F[pointLength - 1] = 0.0f;

    // прогонка
    float* alpha = (float*)malloc(sizeof(float) * (pointLength));
    float* beta  = (float*)malloc(sizeof(float) * (pointLength));

    alpha[1] = - vc[0] / vb[0];
    beta[1] = F[0] / vb[0];
    for (int i = 1; i < pointLength - 1; i++) {
        alpha[i + 1] = -vc[i] / (va[i] * alpha[i] + vb[i]);
        beta[i + 1] = (F[i] - va[i] * beta[i]) / (va[i] * alpha[i] + vb[i]);
    }
    b[pointLength - 1] = (F[pointLength - 1] - va[pointLength - 1] * beta[pointLength - 1]) / (vb[pointLength - 1] + va[pointLength - 1]  * alpha[pointLength - 1]);
    for (int i = pointLength - 2; i >= 0; i--) {
        b[i] = alpha[i + 1] * b[i + 1] + beta[i + 1];
    }

    free(alpha);
    free(beta);
    free(va);
    free(vb);
    free(vc);
    free(F);
    
    len = pointLength - 1;
    offset = 0;
    for (;(len > 0);)
    {
        size_t l = vsetvl_e32m8(len);
        vA = vle32_v_f32m8(b + offset + 1, l);
        vB = vle32_v_f32m8(b + offset, l);

        vC = vfmul_vf_f32m8(vB, 2, l);
        vC = vfadd_vv_f32m8(vC, vA, l);
        vC = vfmul_vf_f32m8(vC, h/3, l);

        vA = vfsub_vv_f32m8(vA, vB, l);
        vA = vfdiv_vf_f32m8(vA, 3*h, l);

        vTmp = vle32_v_f32m8(pointY + offset + 1, l);
        vPointY = vle32_v_f32m8(pointY + offset, l);
        vTmp = vfsub_vv_f32m8(vTmp, vPointY, l);
        vTmp = vfdiv_vf_f32m8(vTmp, h, l);
        vC = vfsub_vv_f32m8(vTmp, vC, l);

        vse32_v_f32m8(a + offset, vA, l);
        vse32_v_f32m8(c + offset, vC, l);

        len -= l;
        offset += l;
    }
    a[pointLength - 1] = a[pointLength - 2];
    c[pointLength - 1] = c[pointLength - 2] + 2 * b[pointLength - 2] * h + 3 * a[pointLength - 2] * h * h;

    len = length;
    offset = 0;

    for (;(len > 0);)
    {
        size_t l = vsetvl_e32m8(len);
        vSrc = vle32_v_f32m8(src + offset, l);

        vTmp = vfsub_vf_f32m8(vSrc, pointX[0], l);
        vTmp = vfdiv_vf_f32m8(vTmp, h, l);
        vNum = vfcvt_rtz_xu_f_v_u32m8(vTmp, l);
        vNum = vmul_vx_u32m8(vNum, sizeof(float), l);

        vPointX = vluxei32_v_f32m8(pointX, vNum, l);
        vPointY = vluxei32_v_f32m8(pointY, vNum, l);
        vX = vfsub_vv_f32m8(vSrc, vPointX, l);

        vA = vluxei32_v_f32m8(a, vNum, l);
        vB = vluxei32_v_f32m8(b, vNum, l);
        vC = vluxei32_v_f32m8(c, vNum, l);
        vX2 = vfmul_vv_f32m8(vX, vX, l);

        vTmp = vfmadd_vv_f32m8(vC, vX, vPointY, l);
        vDst = vfmadd_vv_f32m8(vA, vX, vB, l);
        vDst = vfmadd_vv_f32m8(vDst, vX2, vTmp, l);

        vse32_v_f32m8(dst + offset, vDst, l);

        len -= l;
        offset += l;
    }

    free(a);
    free(b);
    free(c);
}
