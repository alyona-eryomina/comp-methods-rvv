#ifndef _SLAM_H_
#define _SLAM_H_

#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#if defined(RVV07) || defined(RVV_ASM)
#include <riscv_vector.h>
#endif

#ifdef RVV07
extern uint32_t jacobi_rvv(uint32_t size, double* A, double* b, double* x, double eps);
extern uint32_t gauss_seidel(uint32_t size, double* A, double* b, double* x, double eps);

#define jacobi jacobi_rvv
#elif defined RVV_ASM
extern uint32_t jacobi(uint32_t size, double* A, double* b, double* x, double eps);
extern uint32_t gauss_seidel(uint32_t size, double* A, double* b, double* x, double eps);
#else
extern uint32_t jacobi(uint32_t size, double* A, double* b, double* x, double eps);
extern uint32_t gauss_seidel(uint32_t size, double* A, double* b, double* x, double eps);
#endif

#endif // _SLAM_H_