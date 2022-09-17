cmake_minimum_required(VERSION 3.21)

project(EZ_BITFLAGS
	VERSION 0.4.0
	LANGUAGES CXX
)

set(EZ_BITFLAGS_CONFIG_DIR "share/ez-bitflags" CACHE STRING "The relative directory to install package config files.")

include(GNUInstallDirs)
include(FetchContent)

FetchContent_Declare(
	ez-cmake
	GIT_REPOSITORY "https://github.com/errata-c/ez-cmake.git"
	GIT_TAG "main"
)
FetchContent_MakeAvailable(ez-cmake)


add_library(bitflags INTERFACE)
target_include_directories(bitflags
INTERFACE
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
	"$<INSTALL_INTERFACE:include>"
)
target_compile_features(bitflags INTERFACE cxx_std_17)

add_library(ez::bitflags ALIAS bitflags)


if(PROJECT_IS_TOP_LEVEL)
	include(CTest)
	if(BUILD_TESTING)
		add_subdirectory("test")
	endif()

	install(
	DIRECTORY
		"${CMAKE_CURRENT_SOURCE_DIR}/include/"
	DESTINATION
		"include"
	FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
	)

	install(TARGETS bitflags
		EXPORT bitflags-targets
	)

	install_package(
		NAME "ez-bitflags"
		NAMESPACE "ez::"
		EXPORT "bitflags-targets"
		VERSION "${EZ_BITFLAGS_VERSION}"
		COMPATIBILITY "SameMajorVersion"
		DESTINATION "${EZ_BITFLAGS_CONFIG_DIR}"
		ARCH_INDEPENDENT
	)
endif()