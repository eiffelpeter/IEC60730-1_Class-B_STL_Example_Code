;	AREA    |.text|, CODE, READONLY
	AREA    |.text|, CODE
	EXPORT	IEC60730_CPU_Reg_Test_RunTime
	IMPORT	CPUTestPass
	
pattern1_rt 	EQU		0x55555555
pattern2_rt 	EQU		0xAAAAAAAA

IEC60730_CPU_Reg_Test_RunTime
;push registers to stack
	push	{r0-r7}

;disable all interrupt
	movs    r0,#1
	msr     PRIMASK, r0

;===========R0 Test=============
;R0 test
;compare bit 0-7
	movs	r0, #0xAA
	cmp		r0, #0xAA
	bne     _test_r0_fail_rt
	movs	r0, #0x55
	cmp		r0, #0x55
	bne     _test_r0_fail_rt

;compare bit 8-15
	ldr		r0, =0x0000AA00
	lsrs    r0, r0, #8	
	cmp		r0, #0xAA
	bne     _test_r0_fail_rt
	ldr		r0, =0x00005500
	lsrs    r0, r0, #8	
	cmp		r0, #0x55
	bne     _test_r0_fail_rt

;compare bit 16-23	
	ldr		r0, =0x00AA0000
	lsrs    r0, r0, #16	
	cmp		r0, #0xAA
	bne     _test_r0_fail_rt
	ldr		r0, =0x00550000
	lsrs    r0, r0, #16	
	cmp		r0, #0x55
	bne     _test_r0_fail_rt

;compare bit 24-31
	ldr		r0, =0xAA000000
	lsrs    r0, r0, #24	
	cmp		r0, #0xAA
	bne     _test_r0_fail_rt
	ldr		r0, =0x55000000
	lsrs    r0, r0, #24	
	cmp		r0, #0x55
	bne     _test_r0_fail_rt

;pass
	b		_reg_test_r1_r7_rt
	
_test_r0_fail_rt
;enable interrupt
	movs    r0, #0
	msr     PRIMASK, r0
	
;restore registers
    pop     {r0-r7}

;Branch back
	bx      lr
;===================================

;===========R1 - R7 Test=============
_reg_test_r1_r7_rt
	ldr		r0, =pattern1_rt
	ldr		r1, =pattern1_rt
	cmp		r0, r1
	bne 	_test_r1_r7_fail_rt
	ldr		r0, =pattern2_rt
	ldr		r1, =pattern2_rt
	cmp		r0, r1
	bne 	_test_r1_r7_fail_rt
	
	ldr		r0, =pattern1_rt	
	ldr		r2, =pattern1_rt
	cmp		r0, r2
	bne 	_test_r1_r7_fail_rt
	ldr		r0, =pattern2_rt
	ldr		r2, =pattern2_rt
	cmp		r0, r2
	bne 	_test_r1_r7_fail_rt

	ldr		r0, =pattern1_rt	
	ldr		r3, =pattern1_rt
	cmp		r0, r3
	bne 	_test_r1_r7_fail_rt
	ldr		r0, =pattern2_rt
	ldr		r3, =pattern2_rt
	cmp		r0, r3
	bne 	_test_r1_r7_fail_rt

	ldr		r0, =pattern1_rt	
	ldr		r4, =pattern1_rt
	cmp		r0, r4
	bne 	_test_r1_r7_fail_rt
	ldr		r0, =pattern2_rt
	ldr		r4, =pattern2_rt
	cmp		r0, r4
	bne 	_test_r1_r7_fail_rt
	
	ldr		r0, =pattern1_rt	
	ldr		r5, =pattern1_rt
	cmp		r0, r5
	bne 	_test_r1_r7_fail_rt
	ldr		r0, =pattern2_rt
	ldr		r5, =pattern2_rt
	cmp		r0, r5
	bne 	_test_r1_r7_fail_rt
	
	ldr		r0, =pattern1_rt	
	ldr		r6, =pattern1_rt
	cmp		r0, r6
	bne 	_test_r1_r7_fail_rt
	ldr		r0, =pattern2_rt
	ldr		r6, =pattern2_rt
	cmp		r0, r6
	bne 	_test_r1_r7_fail_rt
	
	ldr		r0, =pattern1_rt	
	ldr		r7, =pattern1_rt
	cmp		r0, r7
	bne 	_test_r1_r7_fail_rt
	ldr		r0, =pattern2_rt
	ldr		r7, =pattern2_rt
	cmp		r0, r7
	bne 	_test_r1_r7_fail_rt
	
	b		_test_cpu_reg_pass_rt
	
_test_r1_r7_fail_rt
;enable interrupt
	movs    r0, #0
	msr     PRIMASK, r0
	
;restore registers
    pop     {r0-r7}

;Branch back
	bx      lr
;===================================

_test_cpu_reg_pass_rt
	ldr     r1, =CPUTestPass
	movs    r0, #0x01
	str     r0, [r1]	

;enable interrupt
	movs    r0, #0
	msr     PRIMASK, r0
	
;restore registers
	pop     {r0-r7}
		
;Branch back
	bx      lr
				
	NOP
	END	