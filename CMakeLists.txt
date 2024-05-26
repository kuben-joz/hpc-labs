cmake_minimum_required (VERSION 3.0)
project (MIMUW-tbb-lab1 CXX)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")

find_package ( TBB REQUIRED )

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-g -Wall")

add_executable(parfor_count parfor_count.cpp)
target_link_libraries(parfor_count tbb_debug)

add_executable(parfor_stats parfor_stats.cpp)
target_link_libraries(parfor_stats tbb_debug)


add_executable(tasks-fib tasks-fib.cpp)
target_link_libraries(tasks-fib tbb_debug)

add_executable(nqueens_count nqueens_count.cpp)
target_link_libraries(nqueens_count tbb_debug)

add_executable(nqueens_report nqueens_report.cpp)
target_link_libraries(nqueens_report tbb_debug)

add_executable(nqueens_seq nqueens_seq.cpp)
target_link_libraries(nqueens_seq tbb_debug)

install(TARGETS DESTINATION .)
