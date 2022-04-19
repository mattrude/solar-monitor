/* File: main.c
 * Author: Matt Rude <matt@mattrude.com>
 * URL: https://github.com/mattrude/solar-monitor
 * Hardware: PIC16F15214
 * License: MIT License
 * Created on February 13, 2022, 1:50 PM
 */

#define _XTAL_FREQ 1000000      // Set CPU to 1Mhz


// PIC16F15214 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FEXTOSC = OFF    // External Oscillator Mode Selection bits (Oscillator not enabled)
#pragma config RSTOSC = HFINTOSC_1MHZ// Power-up Default Value for COSC bits (HFINTOSC (1 MHz))
#pragma config CLKOUTEN = OFF   // Clock Out Enable bit (CLKOUT function is disabled; I/O function on RA4)
#pragma config VDDAR = HI       // VDD Range Analog Calibration Selection bit (Internal analog systems are calibrated for operation between VDD = 2.3V - 5.5V)

// CONFIG2
#pragma config MCLRE = EXTMCLR  // Master Clear Enable bit (If LVP = 0, MCLR pin is MCLR; If LVP = 1, RA3 pin function is MCLR)
#pragma config PWRTS = PWRT_OFF // Power-up Timer Selection bits (PWRT is disabled)
#pragma config WDTE = ON        // WDT Operating Mode bits (WDT enabled regardless of Sleep; SEN bit is ignored)
#pragma config BOREN = NSLEEP   // Brown-out Reset Enable bits (Brown-out Reset enabled while running, disabled in Sleep; SBOREN is ignored)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection bit (Brown-out Reset Voltage (VBOR) set to 1.9V)
#pragma config PPS1WAY = ON     // PPSLOCKED One-Way Set Enable bit (The PPSLOCKED bit can be set once after an unlocking sequence is executed; once PPSLOCKED is set, all future changes to PPS registers are prevented)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable bit (Stack Overflow or Underflow will cause a reset)

// CONFIG3

// CONFIG4
#pragma config BBSIZE = BB512   // Boot Block Size Selection bits (512 words boot block size)
#pragma config BBEN = OFF       // Boot Block Enable bit (Boot Block is disabled)
#pragma config SAFEN = OFF      // SAF Enable bit (SAF is disabled)
#pragma config WRTAPP = OFF     // Application Block Write Protection bit (Application Block is not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block is not write-protected)
#pragma config WRTC = OFF       // Configuration Registers Write Protection bit (Configuration Registers are not write-protected)
#pragma config WRTSAF = OFF     // Storage Area Flash (SAF) Write Protection bit (SAF is not write-protected)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (Low Voltage programming enabled. MCLR/Vpp pin function is MCLR. MCLRE Configuration bit is ignored.)

// CONFIG5
#pragma config CP = OFF         // User Program Flash Memory Code Protection bit (User Program Flash Memory code protection is disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>

uint8_t poweredOn = 0;
uint16_t cntLow = 1;


// The Interrupt Subroutine
void __interrupt() interrupts() {
    
    // External Interrupt detected
    if (INTF==1) {
        CLRWDT();           // Feed the Dog
        LATAbits.LATA0 = 0; // Disable Output
        cntLow = (cntLow + 5);  // Add five to the 'cntLow' variable
        INTF = 0;           // Clear the external interrupt flag
    }
}


int main() {
    // Setup Ports
    PORTA  = 0x00;          // Clear PORTA
    LATA   = 0x00;          // Clear Data Latch
    ANSELA = 0x00;          // Enable digital drivers
    TRISA  = 0b00000100;    // Set Interrupt pin (RA2) as an Input
    
    // Enable External Interrupt (RA2)
    PIE0   = 0b00000001;    // Enable INTE (External Interrupt)
    INTPPS = 0b00000010;    // Set INT pin to RA2
    INTF   = 0;             // Clear the external interrupt flag
    INTCON = 0b10010000;    // Enable GIE (Global Interrupt)
                            //  & PEIE (Peripheral Interrupt)
                            //  & INTEDG (External Interrupt Edge) to rising
    
    // Set Default Values
    LATAbits.LATA0 = 0;     // Start with RA0 LOW
    
    CLRWDT();               // Feed the dog once before we start
    while(1) {
        
        // If: the RA2 pin is High (The MCP102 is in normal mode)
        // - Then: Set RA0 to High (Enable output power to USB)
        // - Else: Set RA0 to Low (Disable output power to USB)
        if (PORTAbits.RA2 == 1) {
            
            // Max the count out at 30 (ie. 30 minutes)
            if (cntLow > 30) { cntLow = 30; }
            
            // If the count is greater then zero, then reduce the value by one.
            // - Else: enable the RA0 pin and go to sleep.
            if (cntLow >= 1) {
                cntLow--;
            } else {
                LATAbits.LATA0 = 1;     // Enable Output
                SLEEP();                // Go to sleep
            }
        } else {
            LATAbits.LATA0 = 0;         // Disable Output
        }
        
        // Wait for 1 minute
        for (uint8_t i=0;i<60;i++){
            CLRWDT();                   // Feed the Dog
            __delay_ms(250);            // Wait a quarter a second
            CLRWDT();                   // Feed the Dog
            __delay_ms(250);            // Wait a quarter a second
            CLRWDT();                   // Feed the Dog
            __delay_ms(250);            // Wait a quarter a second
            CLRWDT();                   // Feed the Dog
            __delay_ms(250);            // Wait a quarter a second
            CLRWDT();                   // Feed the Dog
        }
    }
}
