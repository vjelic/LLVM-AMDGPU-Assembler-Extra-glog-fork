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

set(LLVM-MC ${LLVM_TOOLS_BINARY_DIR}/llvm-mc)
set(AMDPHDRS $<TARGET_FILE:amdphdrs>)
set(AMDPHDRS_DIR $<TARGET_FILE_DIR:amdphdrs>)
set(SP3_TO_MC "${CMAKE_SOURCE_DIR}/bin/sp3_to_mc.pl")

macro(asm_assemble f)
add_custom_command(
  OUTPUT ${f}.o
  COMMAND
    ${CLANG}  -x assembler -target amdgcn--amdhsa -mcpu=fiji -c -o ${f}.o ${CMAKE_CURRENT_SOURCE_DIR}/${f}.s
  DEPENDS ${f}.s
  COMMENT "Assembling ${f}.s to ${f}.o"
)
endmacro(asm_assemble)

macro(asm_link f)
add_custom_command(
  OUTPUT ${f}.co
  COMMAND
    ${CLANG} -target amdgcn--amdhsa ${f}.o -o ${f}.co
  DEPENDS ${f}.o
  COMMENT "Linking ${f}.o to ${f}.co with clang"
)
endmacro(asm_link)

macro(asm_example f)
  asm_assemble(${f})
  asm_link(${f})
  add_custom_target(${f}_co ALL DEPENDS ${f}.co)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${f}.co DESTINATION bin COMPONENT examples)
endmacro(asm_example)

macro(inline_asm_example f)
  add_custom_command(
    OUTPUT ${f}.co
    COMMAND
      ${CLANG} -x cl -Dcl_clang_storage_class_specifiers -target amdgcn--amdhsa -mcpu=fiji -B ${AMDPHDRS_DIR} -o ${f}.co ${CMAKE_CURRENT_SOURCE_DIR}/${f}.cl
    DEPENDS ${f}.cl
    COMMENT "Compiling ${f}.cl to ${f}.co"
  )
  add_custom_target(${f}_co ALL DEPENDS ${f}.co)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${f}.co DESTINATION bin COMPONENT examples)
endmacro(inline_asm_example)

macro(asm_sp3_example f)
  add_custom_command(
    OUTPUT ${f}.s
    COMMAND
      ${PERL_EXECUTABLE} ${SP3_TO_MC} ${CMAKE_CURRENT_SOURCE_DIR}/${f}.sp3 > ${CMAKE_CURRENT_SOURCE_DIR}/${f}.s
    DEPENDS ${f}.sp3
    COMMENT "Translating ${f}.sp3 to ${f}"
  )
  asm_example(${f})
endmacro(asm_sp3_example)

macro(asm_dispatch_example f)
  asm_example(${f})
  add_executable(${f} ${f}.cpp)
  target_link_libraries(${f} examples_common)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${f} DESTINATION bin COMPONENT examples)
  add_test(NAME ${f}
           COMMAND ./${f}
           WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endmacro(asm_dispatch_example)

macro(inline_asm_dispatch_example f)
  if (CLANG)
  inline_asm_example(${f})
  add_executable(${f} ${f}.cpp)
  target_link_libraries(${f} examples_common)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${f} DESTINATION bin COMPONENT examples)
  add_test(NAME ${f}
           COMMAND ./${f}
           WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  endif()
endmacro(inline_asm_dispatch_example)

add_subdirectory(common)
add_subdirectory(sp3)
add_subdirectory(asm-kernel)
add_subdirectory(gfx8)
add_subdirectory(api)
