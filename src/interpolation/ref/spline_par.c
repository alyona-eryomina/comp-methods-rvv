#include "interpolation.h"

extern inline void spline_par(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength, SplineLinParBuf* buf)
{
    float h = pointX[1] - pointX[0];

    uint32_t index;
    buf->b[0] = 0;
    for (index = 1; index < pointLength - 1; ++index)
    {
        buf->b[index] = 2 * (pointY[index] - pointY[index - 1]) / h - buf->b[index - 1];
    }

    for (index = 0; index < pointLength - 1; ++index)
    {
        buf->a[index] = (buf->b[index + 1] - buf->b[index]) / (2*h);
    }

    buf->a[pointLength - 1] = buf->a[pointLength - 2];
    buf->b[pointLength - 1] = buf->b[pointLength - 2] + 2 * buf->a[pointLength - 2] * h;

    for (index = 0; index < length; ++index)
    {
        uint32_t num = (uint32_t)((src[index] - pointX[0]) / h);
        float x = src[index] - pointX[num];
        dst[index] = buf->a[num] * x * x + buf->b[num] * x + pointY[num];
    }
}