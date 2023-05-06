set(CMAKE_CXX_STANDARD 23)
add_library(nw_hooks SHARED network_hooks.cc)
target_link_libraries(nw_hooks reutl)