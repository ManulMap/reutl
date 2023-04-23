cmake_minimum_required(VERSION 3.25)
project(reutl)

set(CMAKE_CXX_STANDARD 23)

add_library(reutl INTERFACE)
target_include_directories(reutl INTERFACE include)


enable_testing()
add_subdirectory(test)