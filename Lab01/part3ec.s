		AREA Myprog, CODE, READONLY
		ENTRY
		EXPORT __main
			
;don't change these addresses!
PCR22 	  EQU 0x4004A058 ;PORTB_PCR22  address
SCGC5 	  EQU 0x40048038 ;SIM_SCGC5    address
PDDR 	  EQU 0x400FF054 ;GPIOB_PDDR   address
PCOR 	  EQU 0x400FF048 ;GPIOB_PCOR   address
PSOR      EQU 0x400FF044 ;GPIOB_PSOR   address

ten	  EQU 0x00000400 ; 1 << 10
eight     EQU 0x00000100 ; 1 << 8
twentytwo EQU 0x00400000 ; 1 << 22

__main	
		BL    		LEDSETUP
		MOV		R0, #7
		BL		testdisp
		MOV		R0, #8
		BL		testdisp
		B 		forever		
testdisp
		PUSH		{LR}
		BL		FIBUNACCI
		BL		MORSIFY
		POP		{LR}
		BX		LR
		
		

; SETS UP THE LED
LEDSETUP
		PUSH  		{R4, R5} 			
		LDR   		R4, =ten 		
		LDR		R5, =SCGC5
		STR		R4, [R5]
				
		LDR   		R4, =eight
		LDR   		R5, =PCR22
		STR   		R4, [R5]
				
		LDR   		R4, =twentytwo
		LDR   		R5, =PDDR
		STR   		R4, [R5]
		POP   		{R4, R5}
		BX    		LR

; R0 <- fibunacci(R0) 
FIBUNACCI
		CMP		R0, #1
		BMI		nlt1
		BEQ		neq1
		BPL		ngt1	
nlt1
		MOV		R0, #0
		BX		LR
neq1
		MOV		R0, #1
		BX		LR
ngt1
		PUSH		{R1-R3,LR}
		SUB		R1, R0, #1
		SUB		R2, R0, #2
		MOV		R0, R1
		BL 		FIBUNACCI
		MOV		R3, R0
		MOV		R0, R2
		BL		FIBUNACCI
		ADD		R0, R0, R3
		POP		{R1-R3,LR}
		BX		LR


; TRANSMIT MULTIDIGIT NUMBER
MORSIFY	
		PUSH		{R0-R5, LR}
		MOV		R4, #10
		MOV		R5, #0
loop
		ADD		R5, R5, #1		; digit accum
		UDIV		R2, R0, R4 		; R2 = R0/R5
		MUL		R3, R2, R4		; R3 = R5*R2
		SUB		R3, R0, R3		; R3 = R0-R3
		PUSH 		{R3}
		CMP		R2, #0			; if divident is zero
		BEQ		boom
		MOV		R0, R2
		B		loop
boom	
		POP		{R1}
		BL 		MORSEDIGIT
		BL		LPAUSE
		SUBS		R5, R5, #1
		BNE		boom
		POP		{R0-R5, LR}
		BX		LR


; ENCODE THE DIGIT AS BINARY AND DISPLAY
MORSEDIGIT								
		PUSH 		{R0-R2, LR}
		MOV 		R0, #31			; m = 0b11111
		CMP		R1, #5
		BMI		negative		; if n<5
		CMP		R1, #5
		BPL     	positive		; if n>=5
negative			
		RSB		R2, R1, #5
		LSL		R0, R0, R2
		B		continue
positive		
		SUB		R2, R1, #5
		LSR		R0, R0, R2
		B		continue
continue						
		BL		DISPLAY
		BL 		LPAUSE
		POP		{R0-R2, LR}
		BX 		LR

 
; DISPLAYS THE DIGIT GIVEN  R0[4:0] 
DISPLAY	
		PUSH 		{R1, LR}
		AND 		R1, R0, #16
		CMP		R1, #16
		BPL		point1
		BL		DASH				; 5th binary = 0
		B		point1c
point1
		BL		DOT
point1c	
		AND 		R1, R0, #8
		CMP		R1, #8
		BPL		point2
		BL		DASH				; 4th binary = 0
		B		point2c
point2
		BL		DOT
point2c				
		AND 		R1, R0, #4
		CMP		R1, #4
		BPL		point3
		BL		DASH				; 3rd binary = 0
		B 		point3c
point3
		BL		DOT
point3c				
		AND 		R1, R0, #2
		CMP		R1, #2
		BPL		point4
		BL		DASH				; 2nd binary = 0
		B		point4c
point4
		BL		DOT
point4c		
		AND 		R1, R0, #1
		CMP		R1, #1
		BPL		point5
		BL		DASH				; 1st binary = 0
		B		point5c
point5
		BL		DOT
point5c	
		POP		{R1, LR}
		BX 		LR


; DISPLAYS A DOT
DOT 			
		PUSH 		{LR}
		BL 		LEDON
		BL 		SPAUSE
		BL 		LEDOFF
		BL 		SPAUSE
		POP		{LR}
		BX 		LR			


; DISPLAYS A DASH
DASH 			
		PUSH 		{LR}
		BL 		LEDON
		BL 		LPAUSE
		BL 		LEDOFF
		BL 		SPAUSE
		POP		{LR}
		BX 		LR


; TURNS ON THE LED     
LEDON				
		PUSH  		{R4, R5}
		LDR   		R4, =twentytwo
		LDR   		R5, =PCOR
		STR   		R4, [R5]
		POP		{R4, R5}
		BX    		LR
	
		
; TURNS OFF THE LED
LEDOFF				
		PUSH  		{R4, R5}
		LDR   		R4, =twentytwo
		LDR   		R5, =PSOR
		STR   		R4, [R5]
		POP   		{R4, R5}
		BX    		LR


; SETS THE LARGE CONSTANT TO COUNTDOWN FROM
LPAUSE 			
		PUSH  		{R0, LR}
		MOV 		R0, #0x480000	
		BL		PAUSE
		POP 		{R0, LR}
		BX		LR


; SETS THE SMALL CONSTANT TO COUNTDOWN FROM
SPAUSE			
		PUSH  		{R0, LR}
		MOV 		R0, #0x180000	
		BL		PAUSE
		POP 		{R0, LR}
		BX		LR


; COUNTS DOWN A GIVEN VALUE VALUE IN R0
PAUSE			
		SUBS 		R0, #1 
		BNE 		PAUSE
		BX		LR

				
forever			
		B		forever			; wait here forever	
		END