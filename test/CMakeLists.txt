find_package(Catch2 3 REQUIRED)

link_libraries(Catch2::Catch2WithMain reutl)

add_executable(test_addr test_addr.cc)

add_executable(test_scanner test_scanner.cc)

add_executable(test_vmhk test_vmhk.cc)

include(CTest)
include(Catch)
catch_discover_tests(test_addr)
catch_discover_tests(test_scanner)
catch_discover_tests(test_vmhk)