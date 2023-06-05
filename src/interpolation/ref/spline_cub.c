#include "comp_methods_ref.h"

extern void refSplineCub(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength)
{
    float* a = (float*)malloc(sizeof(float) * (pointLength));
    float* b = (float*)malloc(sizeof(float) * (pointLength));
    float* c = (float*)malloc(sizeof(float) * (pointLength));

    float h = pointX[1] - pointX[0];
    uint32_t index;

    // compute b
    float* va = (float*)malloc(sizeof(float) * (pointLength));
    float* vb = (float*)malloc(sizeof(float) * (pointLength));
    float* vc = (float*)malloc(sizeof(float) * (pointLength));
    float* F  = (float*)malloc(sizeof(float) * (pointLength));
    float coef = 3/(h*h);
    for (index = 1; index < pointLength - 1; index++)
    {
        va[index] = 1.0f;
        vb[index] = 4.0f;
        vc[index] = 1.0f;
        F[index] = coef * (pointY[index-1] - 2*pointY[index] + pointY[index+1]);
    }
    va[0] = 0.0f; vb[0] = 1.0f; vc[0] = 0.0f; F[0] = 0.0f;
    va[pointLength - 1] = 0.0f;
    vb[pointLength - 1] = 1.0f;
    vc[pointLength - 1] = 0.0f;
    F[pointLength - 1] = 0.0f;

    // прогонка
    float* alpha = (float*)malloc(sizeof(float) * (pointLength));
    float* beta  = (float*)malloc(sizeof(float) * (pointLength));

    alpha[1] = - vc[0] / vb[0];
    beta[1] = F[0] / vb[0];
    for (int i = 1; i < pointLength - 1; i++) {
        alpha[i + 1] = -vc[i] / (va[i] * alpha[i] + vb[i]);
        beta[i + 1] = (F[i] - va[i] * beta[i]) / (va[i] * alpha[i] + vb[i]);
    }
    b[pointLength - 1] = (F[pointLength - 1] - va[pointLength - 1] * beta[pointLength - 1]) / (vb[pointLength - 1] + va[pointLength - 1]  * alpha[pointLength - 1]);
    for (int i = pointLength - 2; i >= 0; i--) {
        b[i] = alpha[i + 1] * b[i + 1] + beta[i + 1];
    }

    free(alpha);
    free(beta);
    free(va);
    free(vb);
    free(vc);
    free(F);
    
    for (index = 0; index < pointLength - 1; ++index)
    {
        a[index] = (b[index+1] - b[index]) / (3*h);
        c[index] = (pointY[index+1] - pointY[index]) / h - h * (2*b[index] + b[index+1]) / 3;
    }
    a[pointLength - 1] = a[pointLength - 2];
    c[pointLength - 1] = c[pointLength - 2] + 2 * b[pointLength - 2] * h + 3 * a[pointLength - 2] * h * h;

    for (index = 0; index < length; ++index)
    {
        uint32_t num = (uint32_t)((src[index] - pointX[0]) / h);
        float x = src[index] - pointX[num];
        dst[index] = x * x * (a[num] * x + b[num]) + c[num] * x + pointY[num];
    }

    free(a);
    free(b);
    free(c);
}
