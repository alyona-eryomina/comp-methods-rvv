#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

void newton_equal_nodes_forward(float* src, float* dst, uint32_t length, float pointX, float pointStep, float* pointY, uint32_t pointLength)
{
    float* buf_f = (float*)malloc(sizeof(float) * pointLength);
    float mul = 1.0f;
    float q = 0.0;

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
            buf_f[j] = (buf_f[j] - buf_f[j-1]) / pointStep;
        }
    }

    for (int i = 0; i < length; ++i)
    {
        dst[i] = buf_f[0];
        mul = 1.0f;
        q = (src[i] - pointX) / pointStep;

        for (int j = 1; j < pointLength; ++j)
        {
            mul *= q / j;
            dst[i] += mul * buf_f[j];
            q--;
        }
    }
    free(buf_f);
}

int main()
{
    float pointX = 0;
    float step = 1;
    float pointY[6] = {0, 1, 8, 27, 64, 125};
    float src[2] = {1.5f, 4.5f}, dst[2];
    newton_equal_nodes_forward(src, dst, 2, pointX, step, pointY, 6);
    printf("res = %f, %f\n", dst[0], dst[1]);
    return 0;
}