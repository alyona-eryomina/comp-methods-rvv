#include "interpolation.h"

extern inline void spline_lin_rvv(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength, SplineLinParBuf* buf)
{
    float h = pointX[1] - pointX[0];

    size_t len = pointLength - 1;
    size_t offset = 0;

    vfloat32m1_t vPointY, vPointY1, vPointX;
    vfloat32m1_t vA, vB;
    vfloat32m1_t vSrc, vDst, vTmp;
    vuint32m1_t vNum;

    for (;(len > 0);)
    {
        size_t l = vsetvl_e32m1(len);
        vPointY = vle32_v_f32m1(pointY + offset, l);
        vPointY1 = vle32_v_f32m1(pointY + 1 + offset, l);
        vPointX = vle32_v_f32m1(pointX + offset, l);

        vA = vfsub_vv_f32m1(vPointY1, vPointY, l);
        vA = vfdiv_vf_f32m1(vA, h, l);
        vB = vfnmsub_vv_f32m1(vA, vPointX, vPointY, l);

        vse32_v_f32m1(buf->a + offset, vA, l);
        vse32_v_f32m1(buf->b + offset, vB, l);

        len -= l;
        offset += l;
    }
    buf->a[pointLength - 1] = pointY[pointLength - 1];
    buf->b[pointLength - 1] = buf->b[pointLength - 2];

    len = length;
    offset = 0;

    for (;(len > 0);)
    {
        size_t l = vsetvl_e32m1(len);
        vSrc = vle32_v_f32m1(src + offset, l);

        vTmp = vfsub_vf_f32m1(vSrc, pointX[0], l);
        vTmp = vfdiv_vf_f32m1(vTmp, h, l);
        vNum = vfcvt_xu_f_v_u32m1(vTmp, l);
        vNum = vmul_vx_u32m1(vNum, sizeof(float), l);

        vA = vloxei32_v_f32m1(buf->a, vNum, l);
        vB = vloxei32_v_f32m1(buf->b, vNum, l);
        vDst = vfmadd_vv_f32m1(vA, vSrc, vB, l);

        vse32_v_f32m1(dst + offset, vDst, l);

        len -= l;
        offset += l;
    }
}
