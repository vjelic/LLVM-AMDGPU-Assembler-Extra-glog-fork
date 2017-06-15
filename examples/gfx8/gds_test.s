
.hsa_code_object_version 2,1
.hsa_code_object_isa 8, 0, 3, "AMD", "AMDGPU"

.text
.globl gdsTest
.p2align 8
.type gdsTest,@function
.amdgpu_hsa_kernel gdsTest


.include "gpr_alloc.inc"
kernarg = 0
gid_x = 3
gid_y = 4
gid_z = 5

// kernarg layout:
.set dbg_ptr_off, 0x38

.ifnotdef no_params_file
	.include "params.inc"
.endif

.include "debug.inc"


.GPR_ALLOC_BEGIN

.SGPR_ALLOC_FROM 8
.SGPR_ALLOC_ONCE stmp, 8


.VGPR_ALLOC_FROM 0
.VGPR_ALLOC tid_x
.VGPR_ALLOC vtmp, 4


.if enable_debug_output
	.VGPR_ALLOC dbg_ptr, 2
	.VGPR_ALLOC dbg, 16
	.VGPR_ALLOC dbg_wave_id
	.SGPR_ALLOC dbg_exec_lo
	.SGPR_ALLOC dbg_exec_hi
.endif

.GPR_ALLOC_END


//.text 0
//.p2align 8
gdsTest:

	.amd_kernel_code_t
	 enable_sgpr_kernarg_segment_ptr = 1
	 enable_sgpr_workgroup_id_x = 1
	 enable_sgpr_workgroup_id_y = 1
	 enable_sgpr_workgroup_id_z = 1
	 is_ptr64 = 1
	 granulated_workitem_vgpr_count = .AUTO_VGPR_GRANULATED_COUNT
	 granulated_wavefront_sgpr_count = .AUTO_SGPR_GRANULATED_COUNT
	 enable_vgpr_workitem_id = 1
	 user_sgpr_count = 2
	 //compute_pgm_rsrc2_lds_size = 0
	 kernarg_segment_byte_size = 64
	 wavefront_sgpr_count = .AUTO_SGPR_COUNT
	 workitem_vgpr_count = .AUTO_VGPR_COUNT
	 float_mode = 192
	 workgroup_group_segment_byte_size = 2048 //.AUTO_LDS_BYTE_SIZE
	.end_amd_kernel_code_t

	// debug
	.if enable_debug_output
		s_load_dwordx2 s[6:7], s[kernarg:kernarg+1], 0 + dbg_ptr_off // load debug buffer pointer
		v_lshrrev_b32 v[dbg_wave_id], 6, v[0]
		s_waitcnt 0
		// compute per lane address
		s_mov_b32 s[dbg_exec_lo], exec_lo
		s_mov_b32 s[dbg_exec_hi], exec_hi
		s_mov_b64 exec, -1
		v_mbcnt_lo_u32_b32 v[dbg_ptr], -1, 0
		v_mbcnt_hi_u32_b32 v[dbg_ptr], -1, v[dbg_ptr]
		//v_mov_b32 v[dbg_ptr], v0
		v_mul_u32_u24 v[dbg_ptr], v[dbg_ptr], 4
		v_mov_b32 v[dbg_ptr+1], s[7]
		v_add_u32 v[dbg_ptr], vcc, v[dbg_ptr], s[6]
		v_addc_u32 v[dbg_ptr+1], vcc, v[dbg_ptr+1], 0, vcc
		s_mov_b32 exec_lo, s[dbg_exec_lo]
		s_mov_b32 exec_hi, s[dbg_exec_hi]
		s_mov_b32 s[gid_x], debug_gid_x //debug output batch
		s_mov_b32 s[gid_y], debug_gid_y //debug line batch
		s_mov_b32 s[gid_z], debug_gid_z //debug image
	.endif
	
	s_mov_b32 m0, -1
	
	v_mov_b32 v2, 1
	v_mov_b32 v3, 0
	
	//ds_write_b32 v3, v3 gds
	//s_waitcnt 0
	
	ds_add_u32 v3, v2 gds
	s_waitcnt 0
	
	ds_read_b32 v4, v3 gds
	dump_fvgpr 2, 3
	dbg_store
	
s_endpgm

.Lfunc_end0:
	.size gdsTest, .Lfunc_end0 - gdsTest