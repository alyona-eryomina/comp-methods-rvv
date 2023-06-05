#include "comp_methods_rvv.h"

extern void SplineLin(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength)
{
    float* a = (float*)malloc(sizeof(float) * (pointLength));
    float* b = (float*)malloc(sizeof(float) * (pointLength));

    float h = pointX[1] - pointX[0];

    size_t len = pointLength - 1;
    size_t offset = 0;

    vfloat32m8_t vPointY, vPointY1, vPointX;
    vfloat32m8_t vA, vB;
    vfloat32m8_t vSrc, vDst, vTmp;
    vuint32m8_t vNum;

    for (;(len > 0);)
    {
        size_t l = vsetvl_e32m8(len);
        vPointY = vle32_v_f32m8(pointY + offset, l);
        vPointY1 = vle32_v_f32m8(pointY + 1 + offset, l);
        vPointX = vle32_v_f32m8(pointX + offset, l);

        vA = vfsub_vv_f32m8(vPointY1, vPointY, l);
        vA = vfdiv_vf_f32m8(vA, h, l);
        vB = vfnmsub_vv_f32m8(vA, vPointX, vPointY, l);

        vse32_v_f32m8(a + offset, vA, l);
        vse32_v_f32m8(b + offset, vB, l);

        len -= l;
        offset += l;
    }
    a[pointLength - 1] = pointY[pointLength - 1];
    b[pointLength - 1] = b[pointLength - 2];

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
        vDst = vfmadd_vv_f32m8(vA, vSrc, vB, l);

        vse32_v_f32m8(dst + offset, vDst, l);

        len -= l;
        offset += l;
    }

    free(a);
    free(b);
}
