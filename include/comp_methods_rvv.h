// Copyright 2023
// Alyona Eryomina

#ifndef _COMP_METHODS_RVV_H_
#define _COMP_METHODS_RVV_H_

#include <stdint.h>
#include <stdlib.h>

#ifdef RVV
#include <riscv_vector.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

// Interpolation

void Lagrange(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength);

void NewtonEqual  (float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength);
void NewtonUnequal(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength);

void SplineLin(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength);
void SplinePar(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength);
void SplineCub(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength);

#ifdef __cplusplus
}
#endif

#endif // _COMP_METHODS_RVV_H_