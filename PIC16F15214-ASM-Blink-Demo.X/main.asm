;
; File:     main.S
; Target:   PIC16F15214
; Author:   Matt Rude <matt@mattrude.com>
; Date:     2022-02-18
; Compiler: pic-as v2.32
; IDE:      MPLABX v6.00
;
; Description:
;

;
;                          PIC16F15214
;                   +----------|_|----------+
;             <>  1 : RA2               RA1 : 18 <> 
;             <>  2 : RA3               RA0 : 17 <> 
;             <>  3 : RA4/T0CKI        OSC1 : 16 <- 4MHz crystal
;    ICSP_VPP ->  4 : MCLR             OSC2 : 15 -> 4MHz crystal     
;         GND ->  5 : GND               VDD : 14 <- 5v0
;             <>  6 : RB0/INT       PGD/RB7 : 13 <> ICSP_PGD
;             <>  7 : RB1           PGC/RB6 : 12 <> ICSP_PGC
;             <>  8 : RB2               RB5 : 11 <> 
;             <>  9 : RB3               RB4 : 10 <> 
;                   +-----------------------:
;                            DIP-18

    PROCESSOR   16F15214
    PAGEWIDTH   132
    RADIX       DEC


; PIC16F15214 Configuration Bit Settings

; Assembly source line config statements

; CONFIG1
  CONFIG  FEXTOSC = OFF         ; External Oscillator Mode Selection bits (Oscillator not enabled)
  CONFIG  RSTOSC = HFINTOSC_1MHZ; Power-up Default Value for COSC bits (HFINTOSC (1 MHz))
  CONFIG  CLKOUTEN = OFF        ; Clock Out Enable bit (CLKOUT function is disabled; I/O function on RA4)
  CONFIG  VDDAR = HI            ; VDD Range Analog Calibration Selection bit (Internal analog systems are calibrated for operation between VDD = 2.3V - 5.5V)

; CONFIG2
  CONFIG  MCLRE = EXTMCLR       ; Master Clear Enable bit (If LVP = 0, MCLR pin is MCLR; If LVP = 1, RA3 pin function is MCLR)
  CONFIG  PWRTS = PWRT_OFF      ; Power-up Timer Selection bits (PWRT is disabled)
  CONFIG  WDTE = OFF            ; WDT Operating Mode bits (WDT disabled; SEN is ignored)
  CONFIG  BOREN = ON            ; Brown-out Reset Enable bits (Brown-out Reset Enabled, SBOREN bit is ignored)
  CONFIG  BORV = LO             ; Brown-out Reset Voltage Selection bit (Brown-out Reset Voltage (VBOR) set to 1.9V)
  CONFIG  PPS1WAY = ON          ; PPSLOCKED One-Way Set Enable bit (The PPSLOCKED bit can be set once after an unlocking sequence is executed; once PPSLOCKED is set, all future changes to PPS registers are prevented)
  CONFIG  STVREN = ON           ; Stack Overflow/Underflow Reset Enable bit (Stack Overflow or Underflow will cause a reset)

; CONFIG3

; CONFIG4
  CONFIG  BBSIZE = BB512        ; Boot Block Size Selection bits (512 words boot block size)
  CONFIG  BBEN = OFF            ; Boot Block Enable bit (Boot Block is disabled)
  CONFIG  SAFEN = OFF           ; SAF Enable bit (SAF is disabled)
  CONFIG  WRTAPP = OFF          ; Application Block Write Protection bit (Application Block is not write-protected)
  CONFIG  WRTB = OFF            ; Boot Block Write Protection bit (Boot Block is not write-protected)
  CONFIG  WRTC = OFF            ; Configuration Registers Write Protection bit (Configuration Registers are not write-protected)
  CONFIG  WRTSAF = OFF          ; Storage Area Flash (SAF) Write Protection bit (SAF is not write-protected)
  CONFIG  LVP = ON              ; Low Voltage Programming Enable bit (Low Voltage programming enabled. MCLR/Vpp pin function is MCLR. MCLRE Configuration bit is ignored.)

; CONFIG5
  CONFIG  CP = OFF              ; User Program Flash Memory Code Protection bit (User Program Flash Memory code protection is disabled)

// config statements should precede project file includes.
#include <xc.inc>

  skipnc  MACRO
    btfsc   STATUS,STATUS_C_POSITION
  ENDM

  skipnz  MACRO
    btfsc   STATUS,STATUS_Z_POSITION
  ENDM
;
; Power-On-Reset entry point
;
    PSECT   Por_Vec,global,class=CODE,delta=2
    global  resetVec
resetVec:
    PAGESEL main                ;jump to the main routine
    goto    main

;
;   Data space use by interrupt handler to save context
    PSECT   Isr_Data,global,class=RAM,space=1,delta=1,noexec
;
    GLOBAL  WREG_save,STATUS_save
;
WREG_save:      DS  1
STATUS_save:    DS  1
PCLATH_save:    DS  1
;
;   Interrupt vector and handler
    PSECT   Isr_Vec,global,class=CODE,delta=2
    GLOBAL  IsrVec
;
IsrVec:
    movwf   WREG_save
    swapf   STATUS,W
    movwf   STATUS_save
    movf    PCLATH,W
    movwf   PCLATH_save
;
IsrHandler:
;
IsrExit:
    movf    PCLATH_save,W
    movwf   PCLATH
    swapf   STATUS_save,W
    movwf   STATUS
    swapf   WREG_save,F
    swapf   WREG_save,W
    retfie                      ; Return from interrupt
    

;objects in bank 0 memory
    PSECT   MainData,global,class=RAM,space=1,delta=1,noexec
max:    DS      1               ;reserve 1 byte for max
tmp:    DS      1               ;reserve 1 byte for tmp

/* find the highest PORTB value read, storing this into the object max */
    PSECT   MainCode,global,class=CODE,delta=2
main:
    BANKSEL TRISA               ;starting point
    movlw   0xFF
    movwf   BANKMASK(TRISA)     ;
    BANKSEL max
    clrf    BANKMASK(max)
loop:
    BANKSEL PORTA               ;read and store port value
    movf    BANKMASK(PORTA),w
    BANKSEL tmp
    movwf   BANKMASK(tmp)
    subwf   max,w               ;is this value larger than max?
    skipnc
    goto    loop                ;no - read again
    movf    BANKMASK(tmp),w     ;yes - record this new high value
    movwf   BANKMASK(max)
    goto    loop                ;read again
    END     resetVec


