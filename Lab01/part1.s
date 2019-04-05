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
		MOV   		R0, #0
		BL 		DISPLAY
		MOV   		R0, #16
		BL 		DISPLAY
		MOV   		R0, #24
		BL 		DISPLAY
		MOV   		R0, #28
		BL 		DISPLAY
		MOV   		R0, #30
		BL 		DISPLAY
		MOV   		R0, #31
		BL 		DISPLAYT
		MOV   		R0, #15
		BL 		DISPLAY
		MOV   		R0, #7
		BL 		DISPLAY
		MOV   		R0, #3
		BL 		DISPLAY
		MOV   		R0, #1
		BL 		DISPLAY
		B 		forever


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