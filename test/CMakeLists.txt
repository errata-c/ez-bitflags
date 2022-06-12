
find_package(Catch2 CONFIG REQUIRED)

add_executable(combined_tests 
	"multi_namespace.cpp"
	"basic.cpp"
	"ops.cpp"
)
target_link_libraries(combined_tests PRIVATE ez::bitflags Catch2::Catch2WithMain)