#include "interpolation.h"

extern inline void spline_par_rvv(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength, SplineLinParBuf* buf)
{
    float h = pointX[1] - pointX[0];

    size_t len = pointLength - 1;
    size_t offset = 0;

    vfloat32m1_t vA, vB, vC, vX;
    vfloat32m1_t vSrc, vSrc2, vDst, vTmp;
    vuint32m1_t vNum;

    uint32_t index;
    buf->b[0] = 0;
    for (index = 1; index < pointLength; ++index)
    {
        buf->b[index] = 2 * (pointY[index] - pointY[index - 1]) / h - buf->b[index - 1];
    }

    for (;(len > 0);)
    {
        size_t l = vsetvl_e32m1(len);
        vA = vle32_v_f32m1(buf->b + offset + 1, l);
        vB = vle32_v_f32m1(buf->b + offset, l);

        vA = vfsub_vv_f32m1(vA, vB, l);
        vA = vfdiv_vf_f32m1(vA, 2*h, l);

        vse32_v_f32m1(buf->a + offset, vA, l);

        len -= l;
        offset += l;
    }

    buf->a[pointLength - 1] = buf->a[pointLength - 2];
    buf->b[pointLength - 1] = buf->b[pointLength - 2] + 2 * buf->a[pointLength - 2] * h;

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
        vC = vloxei32_v_f32m1(pointY, vNum, l);
        vX = vloxei32_v_f32m1(pointX, vNum, l);

        vSrc2 = vfsub_vv_f32m1(vSrc, vX, l);
        vDst = vfmadd_vv_f32m1(vB, vSrc2, vC, l);
        vSrc2 = vfmul_vv_f32m1(vSrc2, vSrc2, l);
        vDst = vfmadd_vv_f32m1(vA, vSrc2, vDst, l);

        vse32_v_f32m1(dst + offset, vDst, l);

        len -= l;
        offset += l;
    }
}
