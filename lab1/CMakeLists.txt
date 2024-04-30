cmake_minimum_required(VERSION 3.24)
project(lab1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-fopenmp")
#set(CMAKE_CXX_STANDARD_LIBRARIES "-lpthread")
add_executable(lab1 main.cpp)
