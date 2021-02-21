
find_package(fmt CONFIG REQUIRED)

add_library(test_framework INTERFACE)
target_link_libraries(test_framework INTERFACE fmt::fmt ez-bitflags)

add_subdirectory("basic")

add_subdirectory("ops")