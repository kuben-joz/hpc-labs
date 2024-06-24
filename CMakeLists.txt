cmake_minimum_required (VERSION 3.0)
project (MIMUW-tbb-lab1 CXX)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")

find_package ( TBB REQUIRED )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-g -Wall")

add_executable(nqueens_count nqueens_count.cpp)
target_link_libraries(nqueens_count tbb_debug)

add_executable(shortest_path shortest_path.cpp)
target_link_libraries(shortest_path tbb_debug)

install(TARGETS DESTINATION .)
