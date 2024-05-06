#ifndef _INTERPOLATION_H_
#define _INTERPOLATION_H_

#include <stdint.h>
#include <stdlib.h>

#if defined(RVV07) || defined(RVV_ASM)
#include <riscv_vector.h>
#endif

typedef struct {
    float* a;  // pointLength
    float* b;  // pointLength
} SplineLinParBuf;

typedef struct {
    float* a;  // pointLength
    float* b;  // pointLength
    float* c;  // pointLength
    float* va;  // pointLength
    float* vb;  // pointLength
    float* vc;  // pointLength
    float* F;  // pointLength
    float* alpha;  // pointLength
    float* beta;  // pointLength
} SplineCubBuf;

void init_lin_par_buf(SplineLinParBuf* buf, uint32_t pointLength);
void init_cub_buf(SplineCubBuf* buf, uint32_t pointLength);

void free_lin_par_buf(SplineLinParBuf* buf);
void free_cub_buf(SplineCubBuf* buf);

#ifdef RVV07
extern void spline_lin_rvv(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength, SplineLinParBuf* buf);
extern void spline_par_rvv(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength, SplineLinParBuf* buf);
extern void spline_cub_rvv(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength, SplineCubBuf* buf);

#define spline_lin spline_lin_rvv
#define spline_par spline_par_rvv
#define spline_cub spline_cub_rvv
#elif defined RVV_ASM
extern void spline_lin_rvv_asm(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength, SplineLinParBuf* buf);
extern void spline_par_rvv_asm(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength, SplineLinParBuf* buf);
extern void spline_cub_rvv_asm(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength, SplineCubBuf* buf);

#define spline_lin spline_lin_rvv_asm
#define spline_par spline_par_rvv_asm
#define spline_cub spline_cub_rvv_asm
#else
extern void spline_lin(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength, SplineLinParBuf* buf);
extern void spline_par(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength, SplineLinParBuf* buf);
extern void spline_cub(float* src, float* dst, uint32_t length, float* pointX, float* pointY, uint32_t pointLength, SplineCubBuf* buf);
#endif

#endif // _INTERPOLATION_H_