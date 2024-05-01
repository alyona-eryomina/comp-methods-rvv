cmake_minimum_required(VERSION 3.10)

project(comp-methods-tests)

set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS)

if(is_top_level)
    find_package(comp-methods REQUIRED)
endif()

add_definitions(-g)

add_executable(${PROJECT_NAME} slam.cpp interpolation.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE comp-methods)
target_link_libraries(${PROJECT_NAME} PUBLIC GTest::gtest_main)
