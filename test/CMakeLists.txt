
find_package(fmt CONFIG REQUIRED)

add_library(test_framework INTERFACE)
target_link_libraries(test_framework INTERFACE fmt::fmt ez-bitflags)

add_executable(basic_test "basic/main.cpp")
target_link_libraries(basic_test PRIVATE test_framework)