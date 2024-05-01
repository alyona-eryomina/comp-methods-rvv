#include "interpolation.h"

extern inline void spline_lin(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength, SplineLinParBuf* buf)
{
    float h = pointX[1] - pointX[0];
    uint32_t index;
    for (index = 0; index < pointLength - 1; ++index)
    {
        buf->a[index] = (pointY[index + 1] - pointY[index]) / h;
        buf->b[index] = pointY[index] - buf->a[index] * pointX[index];
    }
    buf->a[pointLength - 1] = pointY[pointLength - 1];
    buf->b[pointLength - 1] = buf->b[pointLength - 2];

    for (index = 0; index < length; ++index)
    {
        uint32_t num = (uint32_t)((src[index] - pointX[0]) / h);
        dst[index] = buf->a[num] * src[index] + buf->b[num];
    }
}
