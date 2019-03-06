/*******************************************************************************
 * Copyright 2018 Marshall Colville (mjc449@cornell.edu)
 * Original Copyright 2016 Microchip Technology Inc. (www.microchip.com)
 * 
 * Derived from the PIC24FJ256GB210 Explorer bootloader example
 * Modified for the OpenMicroBoard hardware by Marshall Colville
 * 
 * 

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

To request to license the code under the MLA license (www.microchip.com/mla_license), 
please contact mla_licensing@microchip.com
 *******************************************************************************/

#include <p24FJ256GB210.h>
#include <libpic30.h>
#include <system.h>
#include <system_config.h>
#include <usb.h>
#include <xc.h>

/** CONFIGURATION Bits **********************************************/
// CONFIG4

// CONFIG3
#pragma config WPFP = WPFP255           // Write Protection Flash Page Segment Boundary (Highest Page (same as page 170))
#pragma config SOSCSEL = EC             // Secondary Oscillator Power Mode Select->External clock (SCLKI) or Digital I/O mode(
#pragma config WUTSEL = LEG             // Voltage Regulator Wake-up Time Select (Default regulator start-up time is used)
#pragma config ALTPMP = ALPMPDIS        // Alternate PMP Pin Mapping (EPMP pins are in default location mode)
#pragma config WPDIS = WPDIS            // Segment Write Protection Disable (Segmented code protection is disabled)
#pragma config WPCFG = WPCFGDIS         // Write Protect Configuration Page Select (Last page (at the top of program memory) and Flash Configuration Words are not write-protected)
#pragma config WPEND = WPENDMEM         // Segment Write Protection End Page Select (Protected code segment upper boundary is at the last page of program memory; the lower boundary is the code page specified by WPFP)

// CONFIG2
#pragma config POSCMOD = HS             // Primary Oscillator Select (HS Oscillator mode is selected)
#pragma config IOL1WAY = OFF            // IOLOCK One-Way Disabled (The IOLOCK bit (OSCCON<6>) can be set once, provided the unlock sequence has been completed. Once set, the Peripheral Pin Select registers cannot be written to a second time.)
#pragma config OSCIOFNC = OFF           // OSCO Pin Configuration->OSCO/CLKO/RC15 functions as CLKO (FOSC/2)
#pragma config FCKSM = CSECME           // Clock Switching and Fail-Safe Clock Monitor->Clock switching is enabled, Fail-Safe Clock Monitor is enabled
#pragma config FNOSC = FRC              // Initial Oscillator Select->FRC
#pragma config PLL96MHZ = ON            // 96MHz PLL Startup Select (96 MHz PLL is enabled automatically on start-up)
#pragma config PLLDIV = DIV5            // 96 MHz PLL Prescaler Select->Oscillator input is divided by 5 (20 MHz input)
#pragma config IESO = OFF               // Internal External Switchover (IESO mode (Two-Speed Start-up) is disabled)

// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config ALTVREF = ALTVREDIS      // Alternate VREF location Enable (VREF is on a default pin (VREF+ on RA9 and VREF- on RA10))
#pragma config WINDIS = OFF             // Windowed WDT (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
#pragma config FWDTEN = OFF             // Watchdog Timer (Watchdog Timer is disabled)
#pragma config ICS = PGx1               // Emulator Pin Placement Select bits->Emulator functions are shared with PGEC1/PGED1
#pragma config GWRP = OFF               // General Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG port is disabled)

#define ERROR_HANDLER __attribute__((interrupt,no_auto_psv))
#define ERROR_HANDLER_NORETURN ERROR_HANDLER __attribute__((noreturn))
#define FAILSAFE_STACK_GUARDSIZE 8

static uint16_t TRAPS_error_code = -1;

/*********************************************************************
 * Function: void SYSTEM_Initialize( SYSTEM_STATE state )
 *
 * Overview: Initializes the system.
 *
 * PreCondition: None
 *
 * Input:  SYSTEM_STATE - the state to initialize the system into
 *
 * Output: None
 *
 ********************************************************************/
void SYSTEM_Initialize(SYSTEM_STATE state) {
    PIN_MANAGER_Initialize();
    OSCILLATOR_Initialize();

    switch (state) {
        case SYSTEM_STATE_USB_START:
            //Switch to alternate interrupt vector table for bootloader
            INTCON2bits.ALTIVT = 1;

            //To enter the bootloader via hardware we jump A14 to C13 and 
            //toggle the pins a few times
            bool gotoBootloader = true;
            int i;
            _TRISB0 = 0;
            _TRISB1 = 1;
            for (i = 0; i < 8; i++) {
                _LATB0 = i % 2;
                Nop();
                if (_RB1 != _LATB0) {
                    gotoBootloader = false;
                    break;
                }
            }

            //Otherwise the bootloader can be entered by setting a flag in
            //program memory (last word == 0xFFFFFE)
            //Next check for the software flag
            uint16_t keyVal;
            TBLPAG = 0x0002;
            keyVal = __builtin_tblrdl(0xA7FE);

            if (keyVal == 0xFFFE) {
                gotoBootloader = true;
                
                _erase_flash(0x02A7FE);
                _write_flash_word24(0x02A7FE, 0xFFFFFF);
                
            }

            if ((gotoBootloader == false) && ((RCON & 0x83) != 0)) {
                //Switch to app standare IVT for non boot mode
                INTCON2bits.ALTIVT = 0;
                __asm__("goto 0x1800");
            }

            //Make sure that the general purpose output driver multiplexed with
            //the VBUS pin is always consistently configured to be tri-stated in
            //USB applications, so as to avoid any possible contention with the host.
            //(ex: maintain TRISFbits.TRISF7 = 1 at all times).
            TRISFbits.TRISF7 = 1;

            break;

        case SYSTEM_STATE_USB_SUSPEND:
            break;

        case SYSTEM_STATE_USB_RESUME:
            break;
    }
}

void OSCILLATOR_Initialize(void) {
    // CPDIV 1:1; RCDIV FRC/2; DOZE 1:8; DOZEN disabled; ROI disabled; 
    CLKDIV = 0x3100;
    // TUN Center frequency; 
    OSCTUN = 0x0000;
    // CF no clock failure; NOSC PRIPLL; SOSCEN disabled; POSCEN enabled; CLKLOCK unlocked; OSWEN Switch is Complete; 
    __builtin_write_OSCCONH((uint8_t) ((0x0304 >> _OSCCON_NOSC_POSITION) & 0x00FF));
    __builtin_write_OSCCONL((uint8_t) ((0x0304 | _OSCCON_OSWEN_MASK) & 0xFF));
    // Wait for Clock switch to occur
    while (OSCCONbits.OSWEN != 0);
}

void PIN_MANAGER_Initialize(void) {
    /****************************************************************************
     * Setting the Output Latch SFR(s)
     ***************************************************************************/
    LATA = 0x0007;
    LATB = 0x7FFF;
    LATC = 0x0010;
    LATD = 0x7000;
    LATE = 0x0000;
    LATF = 0x012C;
    LATG = 0x11C0;

    /****************************************************************************
     * Setting the GPIO Direction SFR(s)
     ***************************************************************************/
    TRISA = 0xC000;
    TRISB = 0x0003;
    TRISC = 0x9000;
    TRISD = 0x0000;
    TRISE = 0x0000;
    TRISF = 0x012C;
    TRISG = 0x000C;

    /****************************************************************************
     * Setting the Weak Pull Up and Weak Pull Down SFR(s)
     ***************************************************************************/
    CNPD1 = 0x0000;
    CNPD2 = 0x0000;
    CNPD3 = 0x0000;
    CNPD4 = 0x0000;
    CNPD5 = 0x0000;
    CNPD6 = 0x0000;
    CNPU1 = 0x0000;
    CNPU2 = 0x0000;
    CNPU3 = 0x0000;
    CNPU4 = 0x0000;
    CNPU5 = 0x0000;
    CNPU6 = 0x0000;

    /****************************************************************************
     * Setting the Open Drain SFR(s)
     ***************************************************************************/
    ODCA = 0x0000;
    ODCB = 0x0000;
    ODCC = 0x0000;
    ODCD = 0x0000;
    ODCE = 0x0000;
    ODCF = 0x0000;
    ODCG = 0x0000;

    /****************************************************************************
     * Setting the Analog/Digital Configuration SFR(s)
     ***************************************************************************/
    ANSA = 0x0000;
    ANSB = 0x0000;
    ANSC = 0x0000;
    ANSD = 0x0000;
    ANSE = 0x0000;
    ANSF = 0x0000;
    ANSG = 0x0000;
}

void __attribute__((naked, noreturn, weak)) TRAPS_halt_on_error(uint16_t code) {
    TRAPS_error_code = code;
#ifdef __DEBUG    
    __builtin_software_breakpoint();
    /* If we are in debug mode, cause a software breakpoint in the debugger */
#endif
    while (1);

}

/**
 * Sets the stack pointer to a backup area of memory, in case we run into
 * a stack error (in which case we can't really trust the stack pointer)
 */
inline static void use_failsafe_stack(void) {
    static uint8_t failsafe_stack[32];
    asm volatile (
            "   mov    %[pstack], W15\n"
            :
            : [pstack]"r"(failsafe_stack)
            );
    /* Controls where the stack pointer limit is, relative to the end of the
     * failsafe stack
     */
    SPLIM = (uint16_t) (((uint8_t *) failsafe_stack) + sizeof (failsafe_stack)
            - FAILSAFE_STACK_GUARDSIZE);
}

/** Oscillator Fail Trap vector**/
void ERROR_HANDLER_NORETURN _OscillatorFail(void) {
    INTCON1bits.OSCFAIL = 0; //Clear the trap flag
    TRAPS_halt_on_error(TRAPS_OSC_FAIL);
}

/** Stack Error Trap Vector**/
void ERROR_HANDLER_NORETURN _StackError(void) {
    /* We use a failsafe stack: the presence of a stack-pointer error
     * means that we cannot trust the stack to operate correctly unless
     * we set the stack pointer to a safe place.
     */
    use_failsafe_stack();
    INTCON1bits.STKERR = 0; //Clear the trap flag
    TRAPS_halt_on_error(TRAPS_STACK_ERR);
}

/** Address Error Trap Vector**/
void ERROR_HANDLER_NORETURN _AddressError(void) {
    INTCON1bits.ADDRERR = 0; //Clear the trap flag
    TRAPS_halt_on_error(TRAPS_ADDRESS_ERR);
}

/** Math Error Trap Vector**/
void ERROR_HANDLER_NORETURN _MathError(void) {
    INTCON1bits.MATHERR = 0; //Clear the trap flag
    TRAPS_halt_on_error(TRAPS_MATH_ERR);
}


#if defined(USB_INTERRUPT)

void __attribute__((interrupt, auto_psv)) _AltUSB1Interrupt() {
    USBDeviceTasks();
}
#endif



