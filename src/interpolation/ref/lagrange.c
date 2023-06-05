#include "comp_methods_ref.h"

void refLagrange(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength) {
    float x, y;
    float lNumerator, lDenominator;
    float Lnx;
    
    uint32_t index = 0;
    for (index = 0; index < length; index++) {
        x = src[index];
        y = 0.0f;

        uint32_t pointIndex = 0;
        for (pointIndex = 0; pointIndex < pointLength; pointIndex++) {
            lNumerator = 1.0f;
            lDenominator = 1.0f;

            uint32_t j = 0;
            for (j = 0; j < pointLength; j++) {
                if (pointIndex != j) {
                    lNumerator *= x - pointX[j];
                    lDenominator *= pointX[pointIndex] - pointX[j];
                }
            }
            Lnx = lNumerator / lDenominator;
            y += pointY[pointIndex] * Lnx;
        }

        dst[index] = y;
    }
}
