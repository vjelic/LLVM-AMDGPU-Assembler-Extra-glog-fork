LLVM AMDGPU Assembler Extras
============================

#### Overview
This repository contains the following useful items related to AMDGPU ISA assembler:

  * *amdphdrs*: utility to convert ELF produced by llvm-mc into AMD Code Object (v1)
  * *examples/asm-kernel*: example of AMDGPU kernel code 
  * *examples/gfx8/ds_bpermute*: transfer data between lanes in a wavefront with ds_bpermute_b32
  * *examples/gfx8/dpp_reduce*: calculate prefix sum in a wavefront with DPP instructions
  * *examples/gfx8/s_memrealtime*: use s_memrealtime instruction to create a delay
  * *examples/gfx8/s_memrealtime_inline*: inline assembly in OpenCL kernel version of s_memrealtime
  * *examples/api/assemble*: use LLVM API to assemble a kernel
  * *examples/api/disassemble*: use LLVM API to disassemble a stream of instructions
  * *bin/sp3_to_mc.pl*: script to convert some AMD sp3 legacy assembler syntax into LLVM MC
  * *examples/sp3*: examples of sp3 convertable code

At the time of this writing (February 2016), LLVM trunk build and latest ROCR runtime is needed.

Note that LLVM trunk (May or later) now uses lld as linker and produces AMD Code Object (v2).
amdphdrs is now obsolete.


#### Building

Top-level CMakeLists.txt is provided to build everything included. The following CMake variables
should be set:
  * HSA_DIR (default /opt/hsa/bin): path to ROCR Runtime
  * LLVM_DIR: path to LLVM build directory

To build everything, create build directory and run cmake and make:

    mkdir build
    cd build  
    cmake -DLLVM_DIR=/srv/git/llvm.git/build ..
    make

Examples that require clang will only be built if clang is built as part of clang.

#### Use cases

##### Assembling to code object with llvm-mc from command line

The following llvm-mc command line produces ELF object asm.o from assembly source asm.s:

    llvm-mc -arch=amdgcn -mcpu=fiji -filetype=obj -o asm.o asm.s


##### Assembling to raw instruction stream with llvm-mc from command line

It is possible to extract contents of .text section after assembling to code object:

    llvm-mc -arch=amdgcn -mcpu=fiji -filetype=obj -o asm.o asm.s
    objdump -h asm.o | grep .text | awk '{print "dd if='asm.o' of='asm' bs=1 count=$[0x" $3 "] skip=$[0x" $6 "]"}' | bash


##### Disassembling code object from command line

The following command line may be used to dump contents of code object:

    llvm-objdump -disassemble asm.o

This includes text disassembly of .text section.


##### Disassembling raw instruction stream from command line

The following command line may be used to disassemble raw instruction stream (without ELF structure):

    hexdump -v -e '/1 "0x%02X "' asm | llvm-mc -arch=amdgcn -mcpu=fiji -disassemble

Here, hexdump is used to display contents of file in hexadecimal (0x.. form) which
is then consumed by llvm-mc.


#### Assembling source into code object using LLVM API

Refer to *examples/api/assemble*.


#### Disassembling instruction stream using LLVM API

Refer to *examples/api/disassemble*.


##### Using amdphdrs

Given ELF object produced by llvm-mc, amdphdrs produces AMDGPU Code Object version 1.
For example, given assembly source in asm.s, the following will assemble it and link using amdphdrs:

    llvm-mc -arch=amdgcn -mcpu=fiji -filetype=obj -o asm.o asm.s
    andphdrs asm.o asm.co


#### Differences between LLVM AMDGPU Assembler and AMD SP3 assembler
##### Macro support
SP3 supports proprietary set of macros/tools. sp3_to_mc.pl script attempts
to translate them into GAS syntax understood by llvm-mc.

##### flat_atomic_cmpswap instruction has 32-bit destination

LLVM AMDGPU:

    flat_atomic_cmpswap v7, v[9:10], v[7:8]

SP3:

    flat_atomic_cmpswap v[7:8], v[9:10], v[7:8]
