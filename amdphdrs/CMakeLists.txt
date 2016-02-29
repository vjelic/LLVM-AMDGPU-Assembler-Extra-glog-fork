cmake_minimum_required(VERSION 2.6)

if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpic")
endif()

project(amdphdrs)

find_package(LibElf REQUIRED)

include_directories(${LIBELF_INCLUDE_DIRS})

add_executable(amdphdrs amdphdrs.c)
target_link_libraries(amdphdrs elf)
