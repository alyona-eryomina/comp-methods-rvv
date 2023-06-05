#include "comp_methods_rvv.h"

void NewtonUnequal(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength)
{
    float* buf_f = (float*)malloc(sizeof(float) * pointLength);
    
    size_t len = 0;
    size_t offset = 0;
    vfloat32m8_t vDst, vMul, vSrc, vTmp;

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

    len = length;
    offset = 0;

    for (;(len > 0);)
    {
        size_t l = vsetvl_e32m8(len);

        vDst = vfmv_v_f_f32m8(buf_f[0], l);
        vMul = vfmv_v_f_f32m8(1.0f, l);

        vSrc = vle32_v_f32m8(src + offset, l);

        for (int j = 1; j < pointLength; ++j)
        {
            vTmp = vfsub_vf_f32m8(vSrc, pointX[j-1], l);
            vMul = vfmul_vv_f32m8(vMul, vTmp, l);
            vDst = vfmadd_vf_f32m8(vMul, buf_f[j], vDst, l);

            vse32_v_f32m8(dst + offset, vDst, l);
        }

        len -= l;
        offset += l;
    }

    free(buf_f);
}
