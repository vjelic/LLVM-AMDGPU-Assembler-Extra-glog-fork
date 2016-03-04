################################################################################
##
## The University of Illinois/NCSA
## Open Source License (NCSA)
## 
## Copyright (c) 2016, Advanced Micro Devices, Inc. All rights reserved.
## 
## Developed by:
## 
##                 AMD Research and AMD HSA Software Development
## 
##                 Advanced Micro Devices, Inc.
## 
##                 www.amd.com
## 
## Permission is hereby granted, free of charge, to any person obtaining a copy
## of this software and associated documentation files (the "Software"), to
## deal with the Software without restriction, including without limitation
## the rights to use, copy, modify, merge, publish, distribute, sublicense,
## and#or sell copies of the Software, and to permit persons to whom the
## Software is furnished to do so, subject to the following conditions:
## 
##  - Redistributions of source code must retain the above copyright notice,
##    this list of conditions and the following disclaimers.
##  - Redistributions in binary form must reproduce the above copyright
##    notice, this list of conditions and the following disclaimers in
##    the documentation and#or other materials provided with the distribution.
##  - Neither the names of Advanced Micro Devices, Inc,
##    nor the names of its contributors may be used to endorse or promote
##    products derived from this Software without specific prior written
##    permission.
## 
## THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
## IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
## FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
## THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
## OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
## ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
## DEALINGS WITH THE SOFTWARE.
##
################################################################################

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

enable_testing()

add_subdirectory(amdphdrs)
add_subdirectory(examples)
