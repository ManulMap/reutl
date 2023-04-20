cmake_minimum_required(VERSION 3.25)
project(reutl)

set(CMAKE_CXX_STANDARD 23)

add_library(reutl src/memory.cc)
target_include_directories(reutl PUBLIC include)


enable_testing()
add_subdirectory(test)