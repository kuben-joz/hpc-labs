cmake_minimum_required(VERSION 3.24)
project(lab4 CUDA)

set(CMAKE_CUDA_STANDARD 17)

add_executable(lab4 main.cu)

add_executable(raytrace raytrace/raytrace.cu raytrace/common/cpu_bitmap.h raytrace/common/errors.h)

add_executable(raytrace1 raytrace/raytrace1.cu raytrace/common/cpu_bitmap.h raytrace/common/errors.h)

add_executable(sharedsum synchro/sharedsum.cu)

set_target_properties(lab4 PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON)
