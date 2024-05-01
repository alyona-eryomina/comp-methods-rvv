#include "interpolation.h"

extern inline void spline_cub(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength, SplineCubBuf* buf)
{
    float h = pointX[1] - pointX[0];
    uint32_t index;

    float coef = 3/(h*h);
    for (index = 1; index < pointLength - 1; index++)
    {
        buf->va[index] = 1.0f;
        buf->vb[index] = 4.0f;
        buf->vc[index] = 1.0f;
        buf->F[index] = coef * (pointY[index-1] - 2*pointY[index] + pointY[index+1]);
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
    
    for (index = 0; index < pointLength - 1; ++index)
    {
        buf->a[index] = (buf->b[index+1] - buf->b[index]) / (3*h);
        buf->c[index] = (pointY[index+1] - pointY[index]) / h - h * (2*buf->b[index] + buf->b[index+1]) / 3;
    }
    buf->a[pointLength - 1] = buf->a[pointLength - 2];
    buf->c[pointLength - 1] = buf->c[pointLength - 2] + 2 * buf->b[pointLength - 2] * h + 3 * buf->a[pointLength - 2] * h * h;

    for (index = 0; index < length; ++index)
    {
        uint32_t num = (uint32_t)((src[index] - pointX[0]) / h);
        float x = src[index] - pointX[num];
        dst[index] = x * x * (buf->a[num] * x + buf->b[num]) + buf->c[num] * x + pointY[num];
    }
}
