cmake_minimum_required(VERSION 3.10)

project(comp-methods-tests)

set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS)

if(is_top_level)
    find_package(comp-methods REQUIRED)
endif()

include_directories("include/")

# REF

add_executable(${PROJECT_NAME}-ref-lagrange 
               interpolation/ref/test_lagrange.cpp)
target_link_libraries(${PROJECT_NAME}-ref-lagrange 
                      PRIVATE comp-methods-ref)

add_executable(${PROJECT_NAME}-ref-newton-equal 
               interpolation/ref/test_newton_equal.cpp)
target_link_libraries(${PROJECT_NAME}-ref-newton-equal 
                      PRIVATE comp-methods-ref)

add_executable(${PROJECT_NAME}-ref-newton-unequal 
               interpolation/ref/test_newton_unequal.cpp)
target_link_libraries(${PROJECT_NAME}-ref-newton-unequal 
                      PRIVATE comp-methods-ref)   

add_executable(${PROJECT_NAME}-ref-spline-lin 
                      interpolation/ref/test_spline_lin.cpp)
target_link_libraries(${PROJECT_NAME}-ref-spline-lin 
                             PRIVATE comp-methods-ref)

add_executable(${PROJECT_NAME}-ref-spline-par 
               interpolation/ref/test_spline_par.cpp)
target_link_libraries(${PROJECT_NAME}-ref-spline-par 
                      PRIVATE comp-methods-ref)

add_executable(${PROJECT_NAME}-ref-spline-cub 
               interpolation/ref/test_spline_cub.cpp)
target_link_libraries(${PROJECT_NAME}-ref-spline-cub 
                      PRIVATE comp-methods-ref)

# RVV

add_executable(${PROJECT_NAME}-rvv-lagrange 
               interpolation/rvv/test_lagrange.cpp)
target_link_libraries(${PROJECT_NAME}-rvv-lagrange 
                      PRIVATE comp-methods)

add_executable(${PROJECT_NAME}-rvv-newton-equal 
               interpolation/rvv/test_newton_equal.cpp)
target_link_libraries(${PROJECT_NAME}-rvv-newton-equal 
                      PRIVATE comp-methods)

add_executable(${PROJECT_NAME}-rvv-newton-unequal 
               interpolation/rvv/test_newton_unequal.cpp)
target_link_libraries(${PROJECT_NAME}-rvv-newton-unequal 
                      PRIVATE comp-methods)

add_executable(${PROJECT_NAME}-rvv-spline-lin 
               interpolation/rvv/test_spline_lin.cpp)
target_link_libraries(${PROJECT_NAME}-rvv-spline-lin 
                      PRIVATE comp-methods)

add_executable(${PROJECT_NAME}-rvv-spline-par 
               interpolation/rvv/test_spline_par.cpp)
target_link_libraries(${PROJECT_NAME}-rvv-spline-par 
                      PRIVATE comp-methods)

add_executable(${PROJECT_NAME}-rvv-spline-cub 
               interpolation/rvv/test_spline_cub.cpp)
target_link_libraries(${PROJECT_NAME}-rvv-spline-cub 
                      PRIVATE comp-methods)