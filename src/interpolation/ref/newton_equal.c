#include "comp_methods_ref.h"

void refNewtonEqual(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength)
{
    float* buf_f = (float*)malloc(sizeof(float) * pointLength);
    float mul = 1.0f;
    float q = 0.0;

    float h = pointX[1] - pointX[0];

    // таблица вычисляется независимо от числа неизвестных точек
    // fill the table
    for (int i = 0; i < pointLength; ++i)
    {
        buf_f[i] = pointY[i];
    }

    for (int i = 1; i < pointLength; ++i)
    {
        for (int j = pointLength - 1; j >= i; --j)
        {
            buf_f[j] = (buf_f[j] - buf_f[j-1]);
        }
    }

    for (int i = 0; i < length; ++i)
    {
        dst[i] = buf_f[0];
        mul = 1.0f;
        q = (src[i] - pointX[0]) / h;

        for (int j = 1; j < pointLength; ++j)
        {
            mul *= q / j;
            dst[i] += mul * buf_f[j];
            q--;
        }
    }
    free(buf_f);
}
