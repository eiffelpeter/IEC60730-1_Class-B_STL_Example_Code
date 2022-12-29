	AREA    |.text|, CODE, READONLY

	EXPORT	IEC60730_PartRamMarchX_WOM
	IMPORT	IEC60730_FullRamMarchX_WOM
	IMPORT	RAMTestPass
	IMPORT	RAMStartAddr
	IMPORT	RAMEndAddr	
	

;DF: ARTISAN 1

;*******************************************************************************
; Function Name  : IEC60730_PartRamMarchX_WOM
; Description    : Partial RAM MarchX test for start-up
; Input          : R0 .. RAM begin (first address to check), 
;                  R1 .. RAM end (last address to check)
;                  R2 .. Background pattern
; Local          : R3 .. Inverted background pattern
;                  R4 .. keeps test result status
;                  R5  .. pointer to RAM
;                  R6  .. content RAM to compare
; Return         : TEST_SUCCESSFULL (=1)
; WARNING        : All the RAM area including stack is destroyed during this test
;*******************************************************************************/
IEC60730_PartRamMarchX_WOM

	PUSH	{R1-R7}
	MOV		R0, R14		
	PUSH	{R0}		; save R14 (LR) register
	BL		IEC60730_FullRamMarchX_WOM
	POP		{R0}
	MOV		R14, R0		; restore R14 (LR) register
	MOV		R0, R4		; pass test result to R0
	POP  	{R1-R7}	
	BX    	LR          ; return to the caller

	NOP
	END	
