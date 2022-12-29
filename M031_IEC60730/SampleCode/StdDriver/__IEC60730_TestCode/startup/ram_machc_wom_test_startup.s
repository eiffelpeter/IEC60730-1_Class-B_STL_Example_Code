	AREA    |.text|, CODE, READONLY

	EXPORT	IEC60730_FullRamMarchC_WOM
	IMPORT	RAMStartAddr
	IMPORT	RAMEndAddr	

;DF: ARTISAN 1

;*******************************************************************************
; Function Name  : IEC60730_FullRamMarchC_WOM
; Description    : Full RAM MarchC test for start-up
; Input          : R0 .. RAM begin (first address to check), 
;                  R1 .. RAM end (last address to check)
;                  R2 .. Background pattern
; Local          : R3 .. Inverted background pattern
;                  R4 .. keeps test result status
;                  R5 .. pointer to RAM
;                  R6 .. content RAM to compare
;                  R7 .. keep R4
; Return         : TEST_SUCCESSFULL (=1)
; WARNING        : All the RAM area including stack is destroyed during this test
;*******************************************************************************/
IEC60730_FullRamMarchC_WOM

	LDR  	R2, =RAMStartAddr
	LDR		R3, =RAMEndAddr
	LDR   	R0, [R2,#+0]	
	LDR   	R1, [R3,#+0]
	LDR  	R2, =0x00
	MOV		R7, R4			; save R4 to R7
	MOVS  	R4, #0x1		; Test success status by default

	MOVS  	R3, R2          ; setup inverted background pattern
	RSBS  	R3, R3, #0
	SUBS  	R3, R3, #1

	;**************************************************
	;	WOM test
	;**************************************************
	; *** inter-word march test ***

	; *** Step 1 *** 
	; Write background pattern with addresses increasing
	MOVS  	R5, R0
__FULL_WOM_1_LOOP
	CMP   	R5, R1
	BHI   	__FULL_WOM_STEP_2
	STR   	R2, [R5,#+0]
	ADDS  	R5, R5, #+4
	B     	__FULL_WOM_1_LOOP

	; *** Step 2 ***
	; Verify background and write inverted background with addresses increasing
__FULL_WOM_STEP_2
	MOVS  	R5, R0
__FULL_WOM_2_LOOP
	CMP   	R5, R1
	BHI   	__FULL_WOM_STEP_3
	LDR   	R6, [R5,#+0]
	CMP   	R6, R2
	BNE   	__FULL_WOM_ERR
	STR   	R3, [R5,#+0]
	LDR   	R6, [R5,#+4]
	CMP   	R6, R2
	BNE   	__FULL_WOM_ERR
	STR   	R3, [R5,#+4]
	; IF :DEF:ARTISAN
	LDR   	R6, [R5,#+12]
	CMP   	R6, R2
	BNE   	__FULL_WOM_ERR
	STR   	R3, [R5,#+12]
	LDR   	R6, [R5,#+8]
	CMP   	R6, R2
	BNE   	__FULL_WOM_ERR
	STR   	R3, [R5,#+8]
	; ELSE
	;  LDR	R6, [R5,#+8]
	;  CMP  R6, R2
	;  BNE  __FULL_WOM_ERR
	;  STR   R3, [R5,#+8]
	;  LDR   R6, [R5,#+12]
	;  CMP   R6, R2
	;  BNE   __FULL_WOM_ERR
	;  STR   R3, [R5,#+12]
	; ENDIF
	ADDS	R5, R5, #+16
	B     	__FULL_WOM_2_LOOP

	; *** Step 3 ***
	; Verify inverted background and write background with addresses increasing  
__FULL_WOM_STEP_3
	MOVS  	R5, R0
__FULL_WOM_3_LOOP
	CMP   	R5, R1
	BHI   	__FULL_WOM_STEP_4  
	LDR   	R6, [R5,#+0]
	CMP   	R6, R3
	BNE   	__FULL_WOM_ERR
	STR   	R2, [R5,#+0]
	LDR   	R6, [R5,#+4]
	CMP   	R6, R3
	BNE   	__FULL_WOM_ERR
	STR   	R2, [R5,#+4]
	; IF :DEF:ARTISAN
	LDR   	R6, [R5,#+12]
	CMP   	R6, R3
	BNE   	__FULL_WOM_ERR
	STR   	R2, [R5,#+12]
	LDR   	R6, [R5,#+8]
	CMP   	R6, R3
	BNE   	__FULL_WOM_ERR
	STR   	R2, [R5,#+8]
	; ELSE
	; LDR   R6, [R5,#+8]
	; CMP   R6, R3
	; BNE   __FULL_WOM_ERR
	; STR   R2, [R5,#+8]
	; LDR   R6, [R5,#+12]
	; CMP   R6, R3
	; BNE   __FULL_WOM_ERR
	; STR   R2, [R5,#+12]
	; ENDIF
	ADDS	R5, R5, #+16
	B     	__FULL_WOM_3_LOOP

	; *** Step 4 ***
	; Verify background and write inverted background with addresses decreasing  
__FULL_WOM_STEP_4
	MOVS  	R5, R1
	SUBS  	R5, R5, #+15
__FULL_WOM_4_LOOP
	CMP   	R5, R0
	BLO   	__FULL_WOM_STEP_5
	; IF :DEF:ARTISAN
	LDR   	R6, [R5,#+8]
	CMP   	R6, R2
	BNE   	__FULL_WOM_ERR
	STR   	R3, [R5,#+8]
	LDR   	R6, [R5,#+12]
	CMP   	R6, R2
	BNE   	__FULL_WOM_ERR
	STR   	R3, [R5,#+12]
	; ELSE
	;  LDR   R6, [R5,#+12]
	;  CMP   R6, R2
	;  BNE   __FULL_WOM_ERR
	;  STR   R3, [R5,#+12]
	;  LDR   R6, [R5,#+8]
	;  CMP   R6, R2
	;  BNE   __FULL_WOM_ERR
	;  STR   R3, [R5,#+8]
	; ENDIF
	LDR   	R6, [R5,#+4]
	CMP   	R6, R2
	BNE   	__FULL_WOM_ERR
	STR   	R3, [R5,#+4]
	LDR   	R6, [R5,#+0]
	CMP   	R6, R2
	BNE   	__FULL_WOM_ERR
	STR   	R3, [R5,#+0]
	SUBS  	R5, R5, #+16
	B     	__FULL_WOM_4_LOOP

	; *** Step 5 ***
	; Verify inverted background and write background with addresses decreasing 
__FULL_WOM_STEP_5
	MOVS  	R5, R1
	SUBS  	R5, R5, #+15
__FULL_WOM_5_LOOP
	CMP   	R5, R0
	BLO   	__FULL_WOM_STEP_6
	; IF :DEF:ARTISAN
	LDR   	R6, [R5,#+8]
	CMP   	R6, R3
	BNE   	__FULL_WOM_ERR
	STR   	R2, [R5,#+8]
	LDR   	R6, [R5,#+12]
	CMP   	R6, R3
	BNE   	__FULL_WOM_ERR
	STR   	R2, [R5,#+12]
	; ELSE
	;  LDR   R6, [R5,#+12]
	;  CMP   R6, R3
	;  BNE   __FULL_WOM_ERR
	;  STR   R2, [R5,#+12]
	;  LDR   R6, [R5,#+8]
	;  CMP   R6, R3
	;  BNE   __FULL_WOM_ERR
	;  STR   R2, [R5,#+8]
	; ENDIF
	LDR   	R6, [R5,#+4]
	CMP   	R6, R3
	BNE   	__FULL_WOM_ERR
	STR   	R2, [R5,#+4]
	LDR   	R6, [R5,#+0]
	CMP   	R6, R3
	BNE   	__FULL_WOM_ERR
	STR   	R2, [R5,#+0]
	SUBS  	R5, R5, #+16
	B     	__FULL_WOM_5_LOOP

	; *** Step 6 ***
	; Verify background with addresses increasing
__FULL_WOM_STEP_6
	MOVS  	R5, R0
__FULL_WOM_6_LOOP
	CMP   	R5, R1
	BHI   	__FULL_WOM_STEP_7
	LDR   	R6, [R5,#+0]
	CMP   	R6, R2
	BNE   	__FULL_WOM_ERR
	ADDS  	R5, R5, #+4
	B     	__FULL_WOM_6_LOOP

__FULL_WOM_ERR
	MOVS  	R4, #0       ; error result

__FULL_WOM_RET
	MOV		R0, R4		; store test result to R0
	MOV		R4, R7		; restore R4 from R7	
	BX    	LR          ; return to the caller

	;**************************************************
	;	WOM test
	;**************************************************
	; *** intra-word march test ***
	; 1. write 0x5555-5555
	; 2. write 0xAAAA-AAAA
	; 3. verify 0xAAAA-AAAA
	; 4. write 0x5555-55555
	; 5. verify 0x5555-55555

__FULL_WOM_STEP_7
	LDR  	R2, =0xAAAAAAAA
	LDR  	R3, =0x55555555

	MOVS  	R5, R0
__FULL_WOM_7_LOOP
	CMP   	R5, R1
	BHI   	__FULL_WOM_RET
	STR   	R3, [R5,#+0]	
	STR   	R2, [R5,#+0]		
	LDR   	R6, [R5,#+0]
	CMP   	R6, R2
	BNE   	__FULL_WOM_ERR
	STR   	R3, [R5,#+0]
	LDR   	R6, [R5,#+0]
	CMP   	R6, R3
	BNE   	__FULL_WOM_ERR
	STR   	R3, [R5,#+4]
	STR   	R2, [R5,#+4]
	LDR   	R6, [R5,#+4]
	CMP   	R6, R2
	BNE   	__FULL_WOM_ERR
	STR   	R3, [R5,#+4]
	LDR   	R6, [R5,#+4]
	CMP   	R6, R3
	BNE   	__FULL_WOM_ERR
	; IF :DEF:ARTISAN	
	STR   	R3, [R5,#+12]
	STR   	R2, [R5,#+12]
	LDR   	R6, [R5,#+12]
	CMP   	R6, R2
	BNE   	__FULL_WOM_ERR
	STR   	R3, [R5,#+12]
	LDR   	R6, [R5,#+12]
	CMP   	R6, R3
	BNE   	__FULL_WOM_ERR
	STR   	R3, [R5,#+8]
	STR   	R2, [R5,#+8]
	LDR   	R6, [R5,#+8]
	CMP   	R6, R2
	BNE   	__FULL_WOM_ERR
	STR   	R3, [R5,#+8]
	LDR   	R6, [R5,#+8]
	CMP   	R6, R3
	BNE   	__FULL_WOM_ERR
	; ELSE
	;	STR   	R3, [R5,#+8]
	;	STR   	R2, [R5,#+8]
	;	LDR   	R6, [R5,#+8]
	;	CMP   	R6, R2
	;	BNE   	__FULL_WOM_ERR
	;	STR   	R3, [R5,#+8]
	;	LDR   	R6, [R5,#+8]
	;	CMP   	R6, R3
	;	BNE   	__FULL_WOM_ERR
	;	STR   	R3, [R5,#+12]
	;	STR   	R2, [R5,#+12]
	;	LDR   	R6, [R5,#+12]
	;	CMP   	R6, R2
	;	BNE   	__FULL_WOM_ERR
	;	STR   	R3, [R5,#+12]
	;	LDR   	R6, [R5,#+12]
	;	CMP   	R6, R3
	;	BNE   	__FULL_WOM_ERR
	ADDS	R5, R5, #+16
	B     	__FULL_WOM_7_LOOP
	NOP
	
	END	
	