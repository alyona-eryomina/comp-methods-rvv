#include "comp_methods_ref.h"

extern void refSplinePar(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength)
{
    float* a = (float*)malloc(sizeof(float) * (pointLength));
    float* b = (float*)malloc(sizeof(float) * (pointLength));
    float h = pointX[1] - pointX[0];

    uint32_t index;
    b[0] = 0;
    for (index = 1; index < pointLength - 1; ++index)
    {
        b[index] = 2 * (pointY[index] - pointY[index - 1]) / h - b[index - 1];
    }

    for (index = 0; index < pointLength - 1; ++index)
    {
        a[index] = (b[index + 1] - b[index]) / (2*h);
    }

    a[pointLength - 1] = a[pointLength - 2];
    b[pointLength - 1] = b[pointLength - 2] + 2 * a[pointLength - 2] * h;

    for (index = 0; index < length; ++index)
    {
        uint32_t num = (uint32_t)((src[index] - pointX[0]) / h);
        float x = src[index] - pointX[num];
        dst[index] = a[num] * x * x + b[num] * x + pointY[num];
    }

    free(a);
    free(b);
}