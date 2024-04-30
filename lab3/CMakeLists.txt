cmake_minimum_required(VERSION 3.24)
project(lab3 CUDA)

set(CMAKE_CUDA_STANDARD 11)

#set(GCC_CONERAGE_COMPILE_FLAGS "-fsanitize=address")
#add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})

add_executable(lab3 stencil.cu)

set_target_properties(lab3 PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON)
