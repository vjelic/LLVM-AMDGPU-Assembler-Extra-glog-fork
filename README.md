LLVM AMDGPU Assembler Extras
============================

#### Overview
This repository contains the following useful items related to AMDGPU ISA assembler:

  * *amdphdrs*: utility to convert ELF produced by llvm-mc into AMD Code Object (v1)
  * *examples/asm-kernel*: example of AMDGPU kernel code 
  * *examples/gfx8/ds_bpermute*: transfer data between lanes in a wavefront with ds_bpermute_b32
  * *examples/gfx8/dpp_reduce*: calculate prefix sum in a wavefront with DPP instructions
  * *examples/gfx8/s_memrealtmie*: use s_memrealtime instruction to create a delay
  * *bin/sp3_to_mc.pl*: script to convert some AMD sp3 legacy assembler syntax into LLVM MC
  * *examples/sp3*: examples of sp3 convertable code

At the time of this writing (February 2016), LLVM trunk build and latest ROCR runtime is needed.


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


#### Using amdphdrs

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
