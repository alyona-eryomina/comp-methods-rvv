cmake_minimum_required(VERSION 3.10)

project(comp-methods-perf)

set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS)

if(is_top_level)
    find_package(comp-methods REQUIRED)
endif()

include_directories("include/")

add_executable(${PROJECT_NAME}-rvv-lagrange 
               interpolation/perf_lagrange.cpp)
target_link_libraries(${PROJECT_NAME}-rvv-lagrange 
                      PRIVATE comp-methods-ref)
target_link_libraries(${PROJECT_NAME}-rvv-lagrange 
                      PRIVATE comp-methods)

add_executable(${PROJECT_NAME}-rvv-newton-equal 
               interpolation/perf_newton_equal.cpp)
target_link_libraries(${PROJECT_NAME}-rvv-newton-equal 
                      PRIVATE comp-methods-ref)
target_link_libraries(${PROJECT_NAME}-rvv-newton-equal 
                      PRIVATE comp-methods)

add_executable(${PROJECT_NAME}-rvv-newton-unequal 
                interpolation/perf_newton_unequal.cpp)
target_link_libraries(${PROJECT_NAME}-rvv-newton-unequal 
                        PRIVATE comp-methods-ref)
target_link_libraries(${PROJECT_NAME}-rvv-newton-unequal 
                        PRIVATE comp-methods)   
                                           
add_executable(${PROJECT_NAME}-rvv-spline-lin 
               interpolation/perf_spline_lin.cpp)
target_link_libraries(${PROJECT_NAME}-rvv-spline-lin 
                      PRIVATE comp-methods-ref)
target_link_libraries(${PROJECT_NAME}-rvv-spline-lin 
                      PRIVATE comp-methods)

add_executable(${PROJECT_NAME}-rvv-spline-par 
               interpolation/perf_spline_par.cpp)
target_link_libraries(${PROJECT_NAME}-rvv-spline-par 
                      PRIVATE comp-methods-ref)
target_link_libraries(${PROJECT_NAME}-rvv-spline-par 
                      PRIVATE comp-methods)

add_executable(${PROJECT_NAME}-rvv-spline-cub 
               interpolation/perf_spline_cub.cpp)
target_link_libraries(${PROJECT_NAME}-rvv-spline-cub 
                      PRIVATE comp-methods-ref)
target_link_libraries(${PROJECT_NAME}-rvv-spline-cub 
                      PRIVATE comp-methods)
