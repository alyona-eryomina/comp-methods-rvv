#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

void newton_unequal_nodes(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength)
{
    float* buf_f = (float*)malloc(sizeof(float) * pointLength);
    float mul = 1.0f;

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
            buf_f[j] = (buf_f[j] - buf_f[j-1]) / (pointX[i + (j-i)] - pointX[j-i]);
        }
    }

    for (int i = 0; i < length; ++i)
    {
        dst[i] = buf_f[0];
        mul = 1.0f;

        for (int j = 1; j < pointLength; ++j)
        {
            mul *= src[i] - pointX[j-1];
            dst[i] += mul * buf_f[j];
        }
    }
    free(buf_f);
}

int main()
{
    float pointX[4] = {0, 2, 3, 5};
    float pointY[4] = {1, 3, 2, 5};
    float src[1] = {4}, dst[1];
    newton_unequal_nodes(src, dst, 1, pointX, pointY, 4);
    printf("res = %f\n", dst[0]);
    return 0;
}