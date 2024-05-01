// Copyright 2023
// Alyona Eryomina

#ifndef _COMP_METHODS_H_
#define _COMP_METHODS_H_

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum {
    INT_LAGRANGE,
    INT_NEWTON,
    INT_SPLINE_LIN,
    INT_SPLINE_PAR,
    INT_SPLINE_CUB
} InterpolationMethod;

typedef enum {
    SLAM_JACOBI,
    SLAM_GAUSS_SEIDEL
} SlamMethod;

void interpolation(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength, InterpolationMethod method);
uint32_t slam(uint32_t size, double* A, double* b, double* x, double eps, SlamMethod method);

#ifdef __cplusplus
}
#endif

#endif // _COMP_METHODS_H_