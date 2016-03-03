.hsa_code_object_version 1,0
.hsa_code_object_isa 8, 0, 3, "AMD", "AMDGPU"

.hsatext
.p2align 8
.amdgpu_hsa_kernel hello_world

hello_world:

   .amd_kernel_code_t
      enable_sgpr_kernarg_segment_ptr = 1
      is_ptr64 = 1
      compute_pgm_rsrc1_vgprs = 0xf
      compute_pgm_rsrc1_sgprs = 0xf
      compute_pgm_rsrc2_user_sgpr = 2
      kernarg_segment_byte_size = 16
      wavefront_sgpr_count = 8
      workitem_vgpr_count = 8
  .end_amd_kernel_code_t

  s_load_dwordx4  s[0:3], s[0:1], 0x00
  v_lshlrev_b32  v0, 2, v0
  s_waitcnt     lgkmcnt(0)

  v_add_u32     v3, vcc, s2, v0
  v_mov_b32     v4, s3
  v_addc_u32    v4, vcc, v4, 0, vcc

  v_add_u32     v1, vcc, s0, v0
  v_mov_b32     v2, s1
  v_addc_u32    v2, vcc, v2, 0, vcc

  flat_load_dword  v0, v[1:2]
  s_waitcnt     vmcnt(0) & lgkmcnt(0)

  v_add_f32 v1, v0, v0 row_shr:1 bound_ctrl:0
  v_add_f32 v1, v0, v1 row_shr:2 bound_ctrl:0
  v_add_f32 v1, v0, v1 row_shr:3 bound_ctrl:0
  v_nop // Nop required for data hazard in SP
  v_nop // Nop required for data hazard in SP
  v_add_f32 v1, v1, v1 row_shr:4 bank_mask:0xe
  v_nop // Nop required for data hazard in SP
  v_nop // Nop required for data hazard in SP
  v_add_f32 v1, v1, v1 row_shr:8 bank_mask:0xc
  v_nop // Nop required for data hazard in SP
  v_nop // Nop required for data hazard in SP
  v_add_f32 v1, v1, v1 row_bcast:15 row_mask:0xa
  v_nop // Nop required for data hazard in SP
  v_nop // Nop required for data hazard in SP
  v_add_f32 v1, v1, v1 row_bcast:31 row_mask:0xc

  flat_store_dword v[3:4], v1
  s_endpgm
