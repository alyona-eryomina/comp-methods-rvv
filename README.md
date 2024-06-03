# comp-methods-rvv

## Build
1. Set path to toolchain in PATH
2. Run build
''' shell
cmake -B build -DBUILD_FOR_ARCH=<ARCH>
cmake --build build
'''
ARCH:
* RISCV - generic code for RISC-V
* RISCV_RVV - RISC-V code with intrinsic RVV
* RISCV_RVV_ASM - RISC-V code with ASM RVV