LLVM AMDGPU Assembler Extras
============================

This repository contains the following useful items related to AMDGPU ISA assembler:

  * *amdphdrs*: utility to convert ELF produced by llvm-mc into AMD Code Object (v1)
  * *examples/asm-kernel*: example of AMDGPU kernel code 
  * *bin/sp3_to_mc.pl*: script to convert some AMD sp3 legacy assembler syntax into LLVM MC
  * *examples/sp3*: examples of sp3 convertable code

At the time of this writing (February 2016), LLVM trunk build and latest ROCR runtime is needed.

Top-level CMakeLists.txt is provided to build everything included. The following CMake variables
should be set:
  * HSA_DIR (default /opt/hsa/bin): path to ROCR Runtime
  * LLVM_DIR: path to LLVM build directory

To build everything, create build directory and run cmake and make:
    mkdir build
    cd build  
    cmake -DLLVM_DIR=/srv/git/llvm.git/build ..
    make
