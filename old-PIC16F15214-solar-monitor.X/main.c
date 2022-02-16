/* 
 * File: main.c
 * Author: Matt Rude <matt@mattrude.com>
 * URL https://github.com/mattrude/solar-monitor
 * Hardware: PIC16F15214
 * License: MIT License
 * Created on February 13, 2022, 1:50 PM
 */

#define _XTAL_FREQ 1000000  // Set CPU to 1Mhz

// CONFIG1
#pragma config FEXTOSC = OFF
#pragma config RSTOSC = HFINTOSC_1MHZ
#pragma config CLKOUTEN = OFF
#pragma config VDDAR = HI

// CONFIG2
#pragma config MCLRE = EXTMCLR
#pragma config PWRTS = PWRT_OFF
#pragma config WDTE = ON
#pragma config BOREN = NSLEEP
#pragma config BORV = LO

// PIC16F15214 Global Libraries
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>

uint8_t poweredOn = 0;
uint16_t numOfResets = 1;
uint16_t cntLastResets = 0;

void __interrupt() interrupts();

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
                            //  & PEIE?(Peripheral Interrupt)
                            //  & INTEDG (External Interrupt Edge) to rising
    
    // Set Default Values
    LATAbits.LATA0 = 0;     // Start with RA0 LOW
    
    
    CLRWDT();               // Feed the dog once before we start
    while(1) {
        if (PORTAbits.RA2 == 1) {
            if (cntLastResets < 5) {
                if (numOfResets >= 1) {
                    numOfResets--;
                } else {
                    LATAbits.LATA0 = 1;         // Enable Output
                    SLEEP();                    // Go to sleep
                }
            } else {
                // Wait for an additional 4 minutes
                for (uint16_t i=0;i<480;i++){
                    CLRWDT();                   // Feed the Dog
                    __delay_ms(500);            // Wait half a second
                }
                cntLastResets--;
            }
        } else {
            LATAbits.LATA0 = 0;                 // Disable Output
        }
        
        // Wait for 1 minute
        for (uint8_t i=0;i<120;i++){
            CLRWDT();                           // Feed the Dog
            __delay_ms(500);                    // Wait half a second
        }
    }
    return (EXIT_SUCCESS);
}


// The Interrupt Subroutine
void interrupts() {
    
    // External Interrupt detected
    if (INTF==1) {
        LATAbits.LATA0 = 0; // Disable Output
        cntLastResets++;
        numOfResets = (numOfResets + 2);
        INTF = 0;           // Clear the external interrupt flag
    }
}
