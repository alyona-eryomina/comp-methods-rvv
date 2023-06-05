#include "comp_methods_rvv.h"

extern void SplinePar(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength)
{
    float* a = (float*)malloc(sizeof(float) * (pointLength));
    float* b = (float*)malloc(sizeof(float) * (pointLength));

    float h = pointX[1] - pointX[0];

    size_t len = pointLength - 1;
    size_t offset = 0;

    vfloat32m8_t vA, vB, vC, vX;
    vfloat32m8_t vSrc, vSrc2, vDst, vTmp;
    vuint32m8_t vNum;

    uint32_t index;
    b[0] = 0;
    for (index = 1; index < pointLength - 1; ++index)
    {
        b[index] = 2 * (pointY[index] - pointY[index - 1]) / h - b[index - 1];
    }

    for (;(len > 0);)
    {
        size_t l = vsetvl_e32m8(len);
        vA = vle32_v_f32m8(b + offset + 1, l);
        vB = vle32_v_f32m8(b + offset, l);

        vA = vfsub_vv_f32m8(vA, vB, l);
        vA = vfdiv_vf_f32m8(vA, 2*h, l);

        vse32_v_f32m8(a + offset, vA, l);

        len -= l;
        offset += l;
    }

    a[pointLength - 1] = a[pointLength - 2];
    b[pointLength - 1] = b[pointLength - 2] + 2 * a[pointLength - 2] * h;

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

        vA = vluxei32_v_f32m8(a, vNum, l);
        vB = vluxei32_v_f32m8(b, vNum, l);
        vC = vluxei32_v_f32m8(pointY, vNum, l);
        vX = vluxei32_v_f32m8(pointX, vNum, l);

        vSrc2 = vfsub_vv_f32m8(vSrc, vX, l);
        vDst = vfmadd_vv_f32m8(vB, vSrc2, vC, l);
        vSrc2 = vfmul_vv_f32m8(vSrc2, vSrc2, l);
        vDst = vfmadd_vv_f32m8(vA, vSrc2, vDst, l);

        vse32_v_f32m8(dst + offset, vDst, l);

        len -= l;
        offset += l;
    }

    free(a);
    free(b);
}
