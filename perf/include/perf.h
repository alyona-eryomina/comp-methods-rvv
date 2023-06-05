#ifndef _PERF_RVV_H_
#define _PERF_RVV_H_

#include "comp_methods_rvv.h"
#include "comp_methods_ref.h"

#include <vector>
#include <math.h>
#include <random>
#include <iostream>

extern __inline unsigned long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
rdcycle(void)
{
    unsigned long dst;
    asm volatile ("csrrs %0, 0xc00, x0" : "=r" (dst) );
    return dst;
}

#endif // _PERF_RVV_H_
