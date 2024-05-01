#include "interpolation.h"

void inline init_lin_par_buf(SplineLinParBuf* buf, uint32_t pointLength) {
    buf->a = (float*)malloc(sizeof(float) * (pointLength));
    buf->b = (float*)malloc(sizeof(float) * (pointLength));
}

void inline init_cub_buf(SplineCubBuf* buf, uint32_t pointLength) {
    buf->a = (float*)malloc(sizeof(float) * (pointLength));
    buf->b = (float*)malloc(sizeof(float) * (pointLength));
    buf->c = (float*)malloc(sizeof(float) * (pointLength));
    buf->va = (float*)malloc(sizeof(float) * (pointLength));
    buf->vb = (float*)malloc(sizeof(float) * (pointLength));
    buf->vc = (float*)malloc(sizeof(float) * (pointLength));
    buf->F = (float*)malloc(sizeof(float) * (pointLength));
    buf->alpha = (float*)malloc(sizeof(float) * (pointLength));
    buf->beta = (float*)malloc(sizeof(float) * (pointLength));
}

void inline free_lin_par_buf(SplineLinParBuf* buf) {
    free(buf->a);
    free(buf->b);
}

void inline free_cub_buf(SplineCubBuf* buf) {
    free(buf->a);
    free(buf->b);
    free(buf->c);
    free(buf->va);
    free(buf->vb);
    free(buf->vc);
    free(buf->F);
    free(buf->alpha);
    free(buf->beta);
}
