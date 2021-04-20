cmake_minimum_required(VERSION 3.17)

project(EZ_BITFLAGS
VERSION 0.2
LANGUAGES CXX)

include(GNUInstallDirs)
include("cmake/ez-cmake/install-package.cmake")

option(BUILD_TESTS "Build the test executables" ON)
option(SEPARATE_DIRECTORY "Install the package into its own directory" ON)
set(CONFIG_DIR "${CMAKE_INSTALL_DATAROOTDIR}/ez-bitflags" CACHE STRING "The relative directory to install package config files.")



add_library(ez-bitflags INTERFACE)

target_include_directories(ez-bitflags
INTERFACE
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
	"$<INSTALL_INTERFACE:include>"
)
target_compile_features(ez-bitflags INTERFACE cxx_std_17)
set_target_properties(ez-bitflags PROPERTIES EXPORT_NAME "bitflags")

if(BUILD_TESTS)
	add_subdirectory("test")
endif()

if(SEPARATE_DIRECTORY)
	set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/ez-bitflags)
endif()

install(
DIRECTORY 
	"${CMAKE_CURRENT_SOURCE_DIR}/include/"
DESTINATION 
	"${CMAKE_INSTALL_INCLUDEDIR}"
FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" 
)

install(TARGETS ez-bitflags
    EXPORT ez-bitflags-targets
)

install_package(
	NAME "ez-bitflags"
	NAMESPACE "ez::"
	EXPORT "ez-bitflags-targets"
	VERSION "${EZ_BITFLAGS_VERSION}"
	COMPATIBILITY "SameMajorVersion"
	DESTINATION "${CONFIG_DIR}"
	ARCH_INDEPENDENT
)