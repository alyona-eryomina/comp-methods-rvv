#include "comp_methods.h"
#include "interpolation.h"

void interpolation(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength, InterpolationMethod method) {
    SplineLinParBuf bufLP;
    SplineCubBuf bufC;

    switch (method) {
    case INT_SPLINE_LIN:
        init_lin_par_buf(&bufLP, pointLength);
        spline_lin(src, dst, length, pointX, pointY, pointLength, &bufLP);
        free_lin_par_buf(&bufLP);
        break;
    case INT_SPLINE_PAR:
        init_lin_par_buf(&bufLP, pointLength);
        spline_par(src, dst, length, pointX, pointY, pointLength, &bufLP);
        free_lin_par_buf(&bufLP);
        break;
    case INT_SPLINE_CUB:
        init_cub_buf(&bufC, pointLength);
        spline_cub(src, dst, length, pointX, pointY, pointLength, &bufC);
        free_cub_buf(&bufC);
        break;
    default:
        break;
    }
}
