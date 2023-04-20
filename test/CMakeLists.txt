find_package(Catch2 3 REQUIRED)

add_executable(test_addr test_addr.cc ../src/memory.cc)
target_include_directories(test_addr PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(test_addr PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(test_addr)