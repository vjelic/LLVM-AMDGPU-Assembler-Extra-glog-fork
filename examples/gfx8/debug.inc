dbg_i = 0
.macro dbg_store
	.if enable_debug_output
		s_waitcnt 0
		s_mov_b32 s[dbg_exec_lo], exec_lo
		s_mov_b32 s[dbg_exec_hi], exec_hi
		s_mov_b64 exec, -1
		v_cmpx_eq_i32 vcc, v[dbg_wave_id], 0+debug_wid
		i = 0
		.rept 16
			flat_store_dword v[dbg_ptr:dbg_ptr+1], v[dbg+i]
			v_add_u32 v[dbg_ptr], vcc, 0x100, v[dbg_ptr]
			v_addc_u32 v[dbg_ptr+1], vcc, v[dbg_ptr+1], 0, vcc
			i = i + 1
		.endr
		s_waitcnt 0
		s_mov_b32 exec_lo, s[dbg_exec_lo]
		s_mov_b32 exec_hi, s[dbg_exec_hi]
	.endif
.endm

.macro dump_vgpr vgpr, count=1
	.if enable_debug_output
		s_waitcnt 0
		s_mov_b32 s[dbg_exec_lo], exec_lo
		s_mov_b32 s[dbg_exec_hi], exec_hi
		s_mov_b64 exec, -1
		i = 0
		.rept \count
			v_mov_b32 v[dbg+dbg_i], v[\vgpr + i]
			i = i + 1
			dbg_i = dbg_i + 1
			.if dbg_i == 16
				dbg_i = 0x0
			.endif
		.endr
		s_mov_b32 exec_lo, s[dbg_exec_lo]
		s_mov_b32 exec_hi, s[dbg_exec_hi]
	.endif
.endm

.macro dump_fvgpr vgpr, count=1
	.if enable_debug_output
		s_waitcnt 0
		s_mov_b32 s[dbg_exec_lo], exec_lo
		s_mov_b32 s[dbg_exec_hi], exec_hi
		s_mov_b64 exec, -1
		i = 0
		.rept \count
			v_cvt_f32_i32 v[dbg+dbg_i], v[\vgpr + i]
			i = i + 1
			dbg_i = dbg_i + 1
			.if dbg_i == 16
				dbg_i = 0x0
			.endif
		.endr
		s_mov_b32 exec_lo, s[dbg_exec_lo]
		s_mov_b32 exec_hi, s[dbg_exec_hi]
	.endif
.endm

.macro dump_sgpr sgpr, count=1
	.if enable_debug_output
		s_waitcnt 0
		s_mov_b32 s[dbg_exec_lo], exec_lo
		s_mov_b32 s[dbg_exec_hi], exec_hi
		s_mov_b64 exec, -1
		i = 0
		.rept \count
			v_mov_b32 v[dbg+dbg_i], s[\sgpr + i]
			i = i + 1
			dbg_i = dbg_i + 1
			.if dbg_i == 16
				dbg_i = 0x0
			.endif
		.endr
		s_mov_b32 exec_lo, s[dbg_exec_lo]
		s_mov_b32 exec_hi, s[dbg_exec_hi]
	.endif
.endm

.macro dump_fsgpr sgpr, count=1
	.if enable_debug_output
		s_waitcnt 0
		s_mov_b32 s[dbg_exec_lo], exec_lo
		s_mov_b32 s[dbg_exec_hi], exec_hi
		s_mov_b64 exec, -1
		i = 0
		.rept \count
			v_cvt_f32_i32 v[dbg+dbg_i], s[\sgpr + i]
			i = i + 1
			dbg_i = dbg_i + 1
			.if dbg_i == 16
				dbg_i = 0x0
			.endif
		.endr
		s_mov_b32 exec_lo, s[dbg_exec_lo]
		s_mov_b32 exec_hi, s[dbg_exec_hi]
	.endif
.endm