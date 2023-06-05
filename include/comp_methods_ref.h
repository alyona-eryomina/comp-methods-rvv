// Copyright 2023
// Alyona Eryomina

#ifndef _COMP_METHODS_REF_H_
#define _COMP_METHODS_REF_H_

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

// Interpolation

void refLagrange(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength);

void refNewtonEqual  (float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength);
void refNewtonUnequal(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength);

void refSplineLin(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength);
void refSplinePar(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength);
void refSplineCub(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength);

#ifdef __cplusplus
}
#endif

#endif // _COMP_METHODS_REF_H_