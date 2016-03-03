.hsa_code_object_version 1,0
.hsa_code_object_isa 8, 0, 3, "AMD", "AMDGPU"

.hsatext
.p2align 8
.amdgpu_hsa_kernel hello_world

hello_world:

   .amd_kernel_code_t
      enable_sgpr_private_segment_buffer = 1
      enable_sgpr_kernarg_segment_ptr = 1
      is_ptr64 = 1
      compute_pgm_rsrc1_vgprs = 8
      compute_pgm_rsrc1_sgprs = 8
      compute_pgm_rsrc2_user_sgpr = 6
      kernarg_segment_byte_size = 24
      wavefront_sgpr_count = 8
      workitem_vgpr_count = 6
  .end_amd_kernel_code_t

  s_load_dwordx4  s[0:3], s[4:5], 0x00
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
  s_load_dwordx2  s[0:1], s[4:5], 0x10
  s_waitcnt     vmcnt(0) & lgkmcnt(0)
  v_lshlrev_b32  v1, 2, v1
  ds_bpermute_b32  v1, v1, v2
  v_add_u32     v3, vcc, s0, v0
  v_mov_b32     v2, s1
  v_addc_u32    v4, vcc, v2, 0, vcc
  s_waitcnt     lgkmcnt(0)
  flat_store_dword  v[3:4], v1
  s_endpgm
