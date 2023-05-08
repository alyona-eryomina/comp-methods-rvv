#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

extern void spline_lin(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength)
{
    float* a = (float*)malloc(sizeof(float) * (pointLength));
    float* b = (float*)malloc(sizeof(float) * (pointLength));

    float h = pointX[1] - pointX[0];
    uint32_t index;
    for (index = 1; index < pointLength; ++index)
    {
        a[index - 1] = (pointY[index] - pointY[index - 1]) / h;
        b[index - 1] = pointY[index - 1] - a[index - 1] * pointX[index - 1];
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

int main()
{
    float pointX[3] = {0, 2, 4};
    float pointY[3] = {1.5f, 2.3f, 3.4f};
    float src[2] = {0.5f, 3.5f}, dst[2];
    spline_lin(src, dst, 2, pointX, pointY, 3);
    printf("res = %f, %f\n", dst[0], dst[1]);
    return 0;
}
