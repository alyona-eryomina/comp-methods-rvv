#ifndef _SLAM_H_
#define _SLAM_H_

#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#if defined(RVV07) || defined(RVV_ASM)
#include <riscv_vector.h>
#endif

typedef struct {
    double* tmp_x;  // size
#if defined(RVV07) || defined(RVV_ASM)
    uint64_t* index;  // size
#endif
} SLAMJacobiBuf;

typedef struct {
    double* tmp_x;  // size
} SLAMGaussBuf;

typedef struct {
    double* r;  // size
    double* z;  // size
    double* s;  // size
} SLAMGradBuf;

void init_jacobi_buf(SLAMJacobiBuf* buf, uint32_t size);
void free_jacobi_buf(SLAMJacobiBuf* buf);

void init_gauss_buf(SLAMGaussBuf* buf, uint32_t size);
void free_gauss_buf(SLAMGaussBuf* buf);

void init_grad_buf(SLAMGradBuf* buf, uint32_t size);
void free_grad_buf(SLAMGradBuf* buf);

#ifdef RVV07
extern uint32_t jacobi_rvv(uint32_t size, double* A, double* b, double* x, double eps, SLAMJacobiBuf* buf);
extern uint32_t gauss_seidel_rvv(uint32_t size, double* A, double* b, double* x, double eps, SLAMGaussBuf* buf);
extern uint32_t conjugate_gradient_rvv(uint32_t size, double* A, double* b, double* x, double eps, SLAMGradBuf* buf);

#define jacobi jacobi_rvv
#define gauss_seidel gauss_seidel_rvv
#define conjugate_gradient conjugate_gradient_rvv

#elif defined RVV_ASM
extern uint32_t jacobi_rvv_asm(uint32_t size, double* A, double* b, double* x, double eps, SLAMJacobiBuf* buf);
extern uint32_t gauss_seidel_rvv_asm(uint32_t size, double* A, double* b, double* x, double eps, SLAMGaussBuf* buf);
extern uint32_t conjugate_gradient_rvv_asm(uint32_t size, double* A, double* b, double* x, double eps, SLAMGradBuf* buf);

#define jacobi jacobi_rvv_asm
#define gauss_seidel gauss_seidel_rvv_asm
#define conjugate_gradient conjugate_gradient_rvv_asm

#else
extern uint32_t jacobi(uint32_t size, double* A, double* b, double* x, double eps, SLAMJacobiBuf* buf);
extern uint32_t gauss_seidel(uint32_t size, double* A, double* b, double* x, double eps, SLAMGaussBuf* buf);
extern uint32_t conjugate_gradient(uint32_t size, double* A, double* b, double* x, double eps, SLAMGradBuf* buf);
#endif

#endif // _SLAM_H_