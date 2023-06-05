#include "comp_methods_ref.h"

extern void refSplineLin(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength)
{
    float* a = (float*)malloc(sizeof(float) * (pointLength));
    float* b = (float*)malloc(sizeof(float) * (pointLength));

    float h = pointX[1] - pointX[0];
    uint32_t index;
    for (index = 0; index < pointLength - 1; ++index)
    {
        a[index] = (pointY[index + 1] - pointY[index]) / h;
        b[index] = pointY[index] - a[index] * pointX[index];
    }
    a[pointLength - 1] = pointY[pointLength - 1];
    b[pointLength - 1] = b[pointLength - 2];

    for (index = 0; index < length; ++index)
    {
        uint32_t num = (uint32_t)((src[index] - pointX[0]) / h);
        dst[index] = a[num] * src[index] + b[num];
    }

    free(a);
    free(b);
}
