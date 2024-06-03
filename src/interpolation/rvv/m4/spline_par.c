#include "interpolation.h"

extern inline void spline_par_rvv(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength, SplineLinParBuf* buf)
{
    float h = pointX[1] - pointX[0];

    size_t len = pointLength - 1;
    size_t offset = 0;

    vfloat32m4_t vA, vB, vC, vX;
    vfloat32m4_t vSrc, vSrc2, vDst, vTmp;
    vuint32m4_t vNum;

    uint32_t index;
    buf->b[0] = 0;
    for (index = 1; index < pointLength; ++index)
    {
        buf->b[index] = 2 * (pointY[index] - pointY[index - 1]) / h - buf->b[index - 1];
    }

    for (;(len > 0);)
    {
        size_t l = vsetvl_e32m4(len);
        vA = vle32_v_f32m4(buf->b + offset + 1, l);
        vB = vle32_v_f32m4(buf->b + offset, l);

        vA = vfsub_vv_f32m4(vA, vB, l);
        vA = vfdiv_vf_f32m4(vA, 2*h, l);

        vse32_v_f32m4(buf->a + offset, vA, l);

        len -= l;
        offset += l;
    }

    buf->a[pointLength - 1] = buf->a[pointLength - 2];
    buf->b[pointLength - 1] = buf->b[pointLength - 2] + 2 * buf->a[pointLength - 2] * h;

    len = length;
    offset = 0;

    for (;(len > 0);)
    {
        size_t l = vsetvl_e32m4(len);
        vSrc = vle32_v_f32m4(src + offset, l);

        vTmp = vfsub_vf_f32m4(vSrc, pointX[0], l);
        vTmp = vfdiv_vf_f32m4(vTmp, h, l);
        vNum = vfcvt_xu_f_v_u32m4(vTmp, l);
        vNum = vmul_vx_u32m4(vNum, sizeof(float), l);

        vX = vloxei32_v_f32m4(pointX, vNum, l);
        vSrc2 = vfsub_vv_f32m4(vSrc, vX, l);

        vB = vloxei32_v_f32m4(buf->b, vNum, l);
        vC = vloxei32_v_f32m4(pointY, vNum, l);

        vDst = vfmadd_vv_f32m4(vB, vSrc2, vC, l);
        vSrc2 = vfmul_vv_f32m4(vSrc2, vSrc2, l);

        vA = vloxei32_v_f32m4(buf->a, vNum, l);
        vDst = vfmadd_vv_f32m4(vA, vSrc2, vDst, l);

        vse32_v_f32m4(dst + offset, vDst, l);

        len -= l;
        offset += l;
    }
}
