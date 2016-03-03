cmake_minimum_required(VERSION 2.6)

project(LLVM-AMDGPU-Assembler-Extra)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")

find_package(Perl REQUIRED)

find_package(LLVM REQUIRED PATHS ${LLVM_DIR} NO_DEFAULT_PATH)

list(APPEND CMAKE_MODULE_PATH ${LLVM_CMAKE_DIR})
include(AddLLVM)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

set (HSA_ROOT "/opt/hsa" CACHE PATH "HSA runtime path")
set (HSA_HEADER_DIR ${HSA_ROOT}/include CACHE PATH "HSA include path")
find_path(HSA_HEADER hsa.h PATHS ${HSA_HEADER_DIR} NO_DEFAULT_PATH)
find_path(HSA_HEADER hsa.h)
if (NOT HSA_HEADER)
  MESSAGE("HSA header not found. Use -DHSA_HEADER_DIR=<path_to_hsa.h>.")
endif (NOT HSA_HEADER)

find_library(HSA_LIBRARY hsa-runtime64 PATHS ${HSA_LIBRARY_DIR} ${HSA_ROOT}/lib NO_DEFAULT_PATH)
find_library(HSA_LIBRARY hsa-runtime64)
if (HSA_LIBRARY)
  MESSAGE(STATUS "Found HSA library: ${HSA_LIBRARY}")
else (HSA_LIBRARY)
  MESSAGE(STATUS "HSA runtime library not found. Use -DHSA_LIBRARY_DIR=<path_to_libhsa-runtime64.so>.")
endif (HSA_LIBRARY)

SET(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/dist CACHE INTERNAL "Prefix prepended to install directories")

if (UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
endif(UNIX)

add_subdirectory(amdphdrs)
add_subdirectory(examples)
