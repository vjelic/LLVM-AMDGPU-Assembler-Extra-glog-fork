////////////////////////////////////////////////////////////////////////////////
//
// The University of Illinois/NCSA
// Open Source License (NCSA)
// 
// Copyright (c) 2016, Advanced Micro Devices, Inc. All rights reserved.
// 
// Developed by:
// 
//                 AMD Research and AMD HSA Software Development
// 
//                 Advanced Micro Devices, Inc.
// 
//                 www.amd.com
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal with the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
// 
//  - Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimers.
//  - Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimers in
//    the documentation and/or other materials provided with the distribution.
//  - Neither the names of Advanced Micro Devices, Inc,
//    nor the names of its contributors may be used to endorse or promote
//    products derived from this Software without specific prior written
//    permission.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS WITH THE SOFTWARE.
//
////////////////////////////////////////////////////////////////////////////////

//
// Use s_memrealtime instruction to read a 64-bit "real time" counter.
// Kernel will wait in a loop for a certain amount of clocks. The number of
// clocks to wait is passed as kernel argument.
//

.hsa_code_object_version 2,0
.hsa_code_object_isa 8, 0, 3, "AMD", "AMDGPU"

.text
.p2align 8
.amdgpu_hsa_kernel hello_world

hello_world:

   .amd_kernel_code_t
      enable_sgpr_kernarg_segment_ptr = 1
      is_ptr64 = 1
      compute_pgm_rsrc1_vgprs = 8
      compute_pgm_rsrc1_sgprs = 8
      compute_pgm_rsrc2_user_sgpr = 6
      kernarg_segment_byte_size = 8
      wavefront_sgpr_count = 8
      workitem_vgpr_count = 6
  .end_amd_kernel_code_t

  //get start timestamp
  s_memrealtime   s[4:5]
  //read kernel argument
  s_load_dwordx2  s[2:3], s[0:1], 0x00
  s_waitcnt 0
  
  //compute finish time s[0:1]
  s_add_u32 s0, s2, s4
  s_addc_u32 s1, s3, s5


  //while( finish_time > get_current_time() ) {}
loop_start:
  //get current timestamp
  s_memrealtime s[4:5]
  s_waitcnt 0
  
  //we don't have s_cmp_*64 instruction so have to compare u32 parts
  //alternatively we can move data to vgpr and use v_cmp_lt_u64
  s_cmp_lt_u32 s5, s1
  s_cbranch_scc1 loop_start
  s_cmp_gt_u32 s5, s1
  s_cbranch_scc1 loop_end
  s_cmp_lt_u32 s4, s0
  s_cbranch_scc1 loop_start
loop_end:
  
  s_endpgm
