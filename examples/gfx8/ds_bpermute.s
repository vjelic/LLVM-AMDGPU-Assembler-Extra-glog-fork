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
// Use ds_bpermute_b32 instruction to transfer data between
// lanes in a wavefront. Input data, lane index and output
// arrays are passed as kernel arguments.
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
      compute_pgm_rsrc1_vgprs = 1
      compute_pgm_rsrc1_sgprs = 0
      compute_pgm_rsrc2_user_sgpr = 2
      kernarg_segment_byte_size = 24
      wavefront_sgpr_count = 8
      workitem_vgpr_count = 5
  .end_amd_kernel_code_t

  s_load_dwordx2  s[4:5], s[0:1], 0x10
  s_load_dwordx4  s[0:3], s[0:1], 0x00
  v_lshlrev_b32  v0, 2, v0
  s_waitcnt     lgkmcnt(0)
  v_add_u32     v1, vcc, s2, v0
  v_mov_b32     v2, s3
  v_addc_u32    v2, vcc, v2, 0, vcc
  v_add_u32     v3, vcc, s0, v0
  v_mov_b32     v4, s1
  v_addc_u32    v4, vcc, v4, 0, vcc
  flat_load_dword  v1, v[1:2]
  flat_load_dword  v2, v[3:4]
  s_waitcnt     vmcnt(0) & lgkmcnt(0)
  v_lshlrev_b32  v1, 2, v1
  ds_bpermute_b32  v1, v1, v2
  v_add_u32     v3, vcc, s4, v0
  v_mov_b32     v2, s5
  v_addc_u32    v4, vcc, v2, 0, vcc
  s_waitcnt     lgkmcnt(0)
  flat_store_dword  v[3:4], v1
  s_endpgm
