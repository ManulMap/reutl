cmake_minimum_required(VERSION 3.25)
project(reutl)

set(CMAKE_CXX_STANDARD 23)

set(REUTL_SRC
        "src/win/memory.cc"
        "src/win/process.cc"
        "src/scanner.cc"
        "src/safe_vmt.cc")

add_library(reutl ${REUTL_SRC})
target_include_directories(reutl PUBLIC include)

if (BUILD_TESTING)
    add_subdirectory(test)
endif ()