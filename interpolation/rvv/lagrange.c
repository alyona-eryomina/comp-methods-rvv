#include <stdint.h>
#include <riscv_vector.h>
#include <stdio.h>

extern void lagrange(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength) {
    float x, y;
    float lNumerator, lDenominator;
    float Lnx;

    size_t len = length;
    size_t offset = 0;

    vfloat32m8_t vX, vY;
    vfloat32m8_t vLNumerator, vLDenominator;
    vfloat32m8_t vPointXIndex, vPointX, vPointY;
    vfloat32m8_t vLnx;
    vfloat32m8_t v1, v2;

    for (;(len > 0);) {
        size_t l = vsetvl_e32m8(len);
        vX = vle32_v_f32m8(src + offset, l);
        vY = vfmv_v_f_f32m8(0.0f, l);

        uint32_t pointIndex = 0;
        for (pointIndex = 0; pointIndex < pointLength; pointIndex++) {
            vLNumerator = vfmv_v_f_f32m8(1.0f, l);
            vLDenominator = vfmv_v_f_f32m8(1.0f, l);

            vPointXIndex = vfmv_v_f_f32m8(pointX[pointIndex], l);
            uint32_t j = 0;
            for (j = 0; j < pointLength; j++) {
                if (pointIndex != j) {
                    float PointX = pointX[j];
                    v1 = vfsub_vf_f32m8(vX, PointX, l);
                    v2 = vfsub_vf_f32m8(vPointXIndex, PointX, l);
                    vLNumerator = vfmul_vv_f32m8(vLNumerator, v1, l);
                    vLDenominator = vfmul_vv_f32m8(vLDenominator, v2, l);
                }
            }
            vLnx = vfdiv_vv_f32m8(vLNumerator, vLDenominator, l);
            v1 = vfmul_vf_f32m8(vLnx, pointY[pointIndex], l);
            vY = vfadd_vv_f32m8(v1, vY, l);
        }
        vse32_v_f32m8(dst + offset, vY, l);

        len -= l;
        offset += l;
    }
}

int main()
{
    float src[4] = {1, 2, 3, 4};
    float dst[4];
    float pX[5] = {0, 2, 4, 6, 8};
    float pY[5] = {0, 4, 16, 36, 64};
    lagrange(src, dst, 4, pX, pY, 5);

    for (int i = 0; i < 4; i++) {
        printf("%f ", dst[i]);
    }
    printf("\n");

    return 0;
}