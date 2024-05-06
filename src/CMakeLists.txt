set(SOURCE_REF interpolation/ref/spline_lin.c
               interpolation/ref/spline_par.c
               interpolation/ref/spline_cub.c
               slam/ref/jacobi.c
               slam/ref/gauss_seidel.c)

set(SOURCE_RVV interpolation/rvv/spline_lin.c
               interpolation/rvv/spline_par.c
               interpolation/rvv/spline_cub.c
               slam/rvv/jacobi.c
               slam/ref/gauss_seidel.c)

set(SOURCE_ASM interpolation/rvv/asm/spline_lin.s
               interpolation/rvv/asm/spline_par.s
               interpolation/rvv/asm/spline_cub.s
               slam/rvv/jacobi.c
               slam/ref/gauss_seidel.c)

set(SOURCE_MAIN interpolation/init_buf.c
                interpolation.c
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

add_definitions(-g)
include_directories("include/")

add_library(${PROJECT_NAME} STATIC ${SOURCE_MAIN} ${SOURCE})