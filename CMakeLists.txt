cmake_minimum_required(VERSION 3.10)

project(comp-methods)

# set(CMAKE_BUILD_TYPE Release)

# set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)
add_definitions(-DRVV -march=rv64gcv -Wall)

include_directories("include/")

set(SOURCE_LIB src/interpolation/rvv/lagrange.c
               src/interpolation/rvv/newton_equal.c
               src/interpolation/rvv/newton_unequal.c
               src/interpolation/rvv/spline_lin.c
               src/interpolation/rvv/spline_par.c
               src/interpolation/rvv/spline_cub.c)

set(SOURCE_REF src/interpolation/ref/lagrange.c
               src/interpolation/ref/newton_equal.c
               src/interpolation/ref/newton_unequal.c
               src/interpolation/ref/spline_lin.c
               src/interpolation/ref/spline_par.c
               src/interpolation/ref/spline_cub.c)

add_library(${PROJECT_NAME} STATIC ${SOURCE_LIB})

add_library(${PROJECT_NAME}-ref STATIC ${SOURCE_REF})

add_subdirectory(tests)
add_subdirectory(perf)
