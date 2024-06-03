#include "slam.h"

void inline init_jacobi_buf(SLAMJacobiBuf* buf, uint32_t size) {
    buf->tmp_x = (double*)malloc(sizeof(double) * (size));
#if defined(RVV07) || defined(RVV_ASM)
    buf->index = (uint64_t*)malloc(sizeof(uint64_t) * (size));
#endif
}

void inline init_gauss_buf(SLAMGaussBuf* buf, uint32_t size) {
    buf->tmp_x = (double*)malloc(sizeof(double) * (size));
}

void inline init_grad_buf(SLAMGradBuf* buf, uint32_t size) {
    buf->r = (double*)malloc(sizeof(double) * (size));
    buf->z = (double*)malloc(sizeof(double) * (size));
    buf->s = (double*)malloc(sizeof(double) * (size));
}

void inline free_jacobi_buf(SLAMJacobiBuf* buf) {
    free(buf->tmp_x);
#if defined(RVV07) || defined(RVV_ASM)
    free(buf->index);
#endif
}

void inline free_gauss_buf(SLAMGaussBuf* buf) {
    free(buf->tmp_x);
}

void inline free_grad_buf(SLAMGradBuf* buf) {
    free(buf->r);
    free(buf->z);
    free(buf->s);
}