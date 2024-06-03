set(SOURCE_REF interpolation/ref/spline_lin.c
               interpolation/ref/spline_par.c
               interpolation/ref/spline_cub.c
               slam/ref/jacobi.c
               slam/ref/gauss_seidel.c
               slam/ref/conjugate_gradient.c)

set(SOURCE_RVV interpolation/rvv/m1/spline_lin.c
               interpolation/rvv/m1/spline_par.c
               interpolation/rvv/m1/spline_cub.c
               slam/rvv/m1/jacobi.c
               slam/rvv/m1/gauss_seidel.c
               slam/rvv/m1/conjugate_gradient.c)

set(SOURCE_ASM interpolation/rvv/m8/asm/spline_lin.s
               interpolation/rvv/m8/asm/spline_par.s
               interpolation/rvv/m8/asm/spline_cub.s
               slam/rvv/m8/asm/jacobi.s
               slam/rvv/m8/asm/gauss_seidel.s
               slam/rvv/m8/asm/conjugate_gradient.s)

set(SOURCE_MAIN interpolation/init_buf.c
                interpolation.c
                slam/init_buf.c
                slam.c)

if (DEFINED BUILD_FOR_ARCH)
    if(${BUILD_FOR_ARCH} STREQUAL "RISCV_RVV")
        set(SOURCE ${SOURCE_RVV})
        add_definitions(-DRVV07)
    elseif(${BUILD_FOR_ARCH} STREQUAL "RISCV_RVV_ASM")
        set(SOURCE ${SOURCE_ASM})
        add_definitions(-DRVV_ASM)
    else()
        set(SOURCE ${SOURCE_REF})
    endif()
else()
    set(SOURCE ${SOURCE_REF})
endif()

#add_definitions(-g)
include_directories("include/")

add_library(${PROJECT_NAME} STATIC ${SOURCE_MAIN} ${SOURCE})