#include "interpolation.h"

extern inline void spline_cub_rvv(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength, SplineCubBuf* buf)
{
    float h = pointX[1] - pointX[0];

    size_t len = pointLength - 1;
    size_t offset = 0;

    vfloat32m1_t vA, vB, vC, vF, vX, vX2, vPointX, vPointY, vPointY2, vPointY3;
    vfloat32m1_t vSrc, vSrc2, vDst, vTmp;
    vuint32m1_t vNum;

    uint32_t index;

    // compute b
    float coef = 3/(h*h);

    len = pointLength - 1;
    offset = 1;
    for (;(len > 1);)
    {
        size_t l = vsetvl_e32m1(len);

        vA = vfmv_v_f_f32m1(1.0f, l);
        vse32_v_f32m1(buf->va + offset, vA, l);
        vse32_v_f32m1(buf->vc + offset, vA, l);

        vB = vfmv_v_f_f32m1(4.0f, l);
        vse32_v_f32m1(buf->vb + offset, vB, l);

        vPointY = vle32_v_f32m1(pointY + offset - 1, l);
        vPointY2 = vle32_v_f32m1(pointY + offset, l);
        vPointY3 = vle32_v_f32m1(pointY + offset + 1, l);
        vPointY2 = vfmul_vf_f32m1(vPointY2, 2.0f, l);
        vF = vfsub_vv_f32m1(vPointY, vPointY2, l);
        vF = vfadd_vv_f32m1(vF, vPointY3, l);
        vF = vfmul_vf_f32m1(vF, coef, l);
        vse32_v_f32m1(buf->F + offset, vF, l);

        len -= l;
        offset += l;
    }
    buf->va[0] = 0.0f; buf->vb[0] = 1.0f; buf->vc[0] = 0.0f; buf->F[0] = 0.0f;
    buf->va[pointLength - 1] = 0.0f;
    buf->vb[pointLength - 1] = 1.0f;
    buf->vc[pointLength - 1] = 0.0f;
    buf->F[pointLength - 1] = 0.0f;

    // прогонка
    buf->alpha[1] = - buf->vc[0] / buf->vb[0];
    buf->beta[1] = buf->F[0] / buf->vb[0];
    for (int i = 1; i < pointLength - 1; i++) {
        buf->alpha[i + 1] = -buf->vc[i] / (buf->va[i] * buf->alpha[i] + buf->vb[i]);
        buf->beta[i + 1] = (buf->F[i] - buf->va[i] * buf->beta[i]) / (buf->va[i] * buf->alpha[i] + buf->vb[i]);
    }
    buf->b[pointLength - 1] = (buf->F[pointLength - 1] - buf->va[pointLength - 1] * buf->beta[pointLength - 1]) / (buf->vb[pointLength - 1] + buf->va[pointLength - 1]  * buf->alpha[pointLength - 1]);
    for (int i = pointLength - 2; i >= 0; i--) {
        buf->b[i] = buf->alpha[i + 1] * buf->b[i + 1] + buf->beta[i + 1];
    }
    
    len = pointLength - 1;
    offset = 0;
    for (;(len > 0);)
    {
        size_t l = vsetvl_e32m1(len);
        vA = vle32_v_f32m1(buf->b + offset + 1, l);
        vB = vle32_v_f32m1(buf->b + offset, l);

        vC = vfmul_vf_f32m1(vB, 2, l);
        vC = vfadd_vv_f32m1(vC, vA, l);
        vC = vfmul_vf_f32m1(vC, h/3, l);

        vA = vfsub_vv_f32m1(vA, vB, l);
        vA = vfdiv_vf_f32m1(vA, 3*h, l);

        vTmp = vle32_v_f32m1(pointY + offset + 1, l);
        vPointY = vle32_v_f32m1(pointY + offset, l);
        vTmp = vfsub_vv_f32m1(vTmp, vPointY, l);
        vTmp = vfdiv_vf_f32m1(vTmp, h, l);
        vC = vfsub_vv_f32m1(vTmp, vC, l);

        vse32_v_f32m1(buf->a + offset, vA, l);
        vse32_v_f32m1(buf->c + offset, vC, l);

        len -= l;
        offset += l;
    }
    buf->a[pointLength - 1] = buf->a[pointLength - 2];
    buf->c[pointLength - 1] = buf->c[pointLength - 2] + 2 * buf->b[pointLength - 2] * h + 3 * buf->a[pointLength - 2] * h * h;

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

        vPointX = vloxei32_v_f32m1(pointX, vNum, l);
        vPointY = vloxei32_v_f32m1(pointY, vNum, l);
        vX = vfsub_vv_f32m1(vSrc, vPointX, l);

        vA = vloxei32_v_f32m1(buf->a, vNum, l);
        vB = vloxei32_v_f32m1(buf->b, vNum, l);
        vC = vloxei32_v_f32m1(buf->c, vNum, l);
        vX2 = vfmul_vv_f32m1(vX, vX, l);

        vTmp = vfmadd_vv_f32m1(vC, vX, vPointY, l);
        vDst = vfmadd_vv_f32m1(vA, vX, vB, l);
        vDst = vfmadd_vv_f32m1(vDst, vX2, vTmp, l);

        vse32_v_f32m1(dst + offset, vDst, l);

        len -= l;
        offset += l;
    }
}
