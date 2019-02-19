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
*******************************************************************************/

#include <p24FJ256GB210.h>
#include "system.h"
#include "../usb/usb.h"
#include "peripherals/waveform_generators.h"
#include "peripherals/waveform_dac.h"


/** CONFIGURATION Bits **********************************************/
// CONFIG4

// CONFIG3
#pragma config WPFP = WPFP255    // Write Protection Flash Page Segment Boundary->Highest Page (same as page 170)
#pragma config SOSCSEL = EC    // Secondary Oscillator Power Mode Select->External clock (SCLKI) or Digital I/O mode(
#pragma config WUTSEL = LEG    // Voltage Regulator Wake-up Time Select->Default regulator start-up time is used
#pragma config ALTPMP = ALPMPDIS    // Alternate PMP Pin Mapping->EPMP pins are in default location mode
#pragma config WPDIS = WPDIS    // Segment Write Protection Disable->Segmented code protection is disabled
#pragma config WPCFG = WPCFGDIS    // Write Protect Configuration Page Select->Last page (at the top of program memory) and Flash Configuration Words are not write-protected
#pragma config WPEND = WPENDMEM    // Segment Write Protection End Page Select->Protected code segment upper boundary is at the last page of program memory; the lower boundary is the code page specified by WPFP

// CONFIG2
#pragma config POSCMOD = HS    // Primary Oscillator Select->HS Oscillator mode is selected
#pragma config IOL1WAY = OFF    // IOLOCK One-Way Set Enable->The IOLOCK bit can be set and cleared as needed, provided the unlock sequence has been completed
#pragma config OSCIOFNC = OFF    // OSCO Pin Configuration->OSCO/CLKO/RC15 functions as CLKO (FOSC/2)
#pragma config FCKSM = CSECME    // Clock Switching and Fail-Safe Clock Monitor->Clock switching is enabled, Fail-Safe Clock Monitor is enabled
#pragma config FNOSC = FRC    // Initial Oscillator Select->FRC
#pragma config PLL96MHZ = ON    // 96MHz PLL Startup Select->96 MHz PLL is enabled automatically on start-up
#pragma config PLLDIV = DIV5    // 96 MHz PLL Prescaler Select->Oscillator input is divided by 5 (20 MHz input)
#pragma config IESO = ON    // Internal External Switchover->IESO mode (Two-Speed Start-up) is enabled

// CONFIG1
#pragma config WDTPS = PS16384    // Watchdog Timer Postscaler->1:16384
#pragma config FWPSA = PR128    // WDT Prescaler->Prescaler ratio of 1:128
#pragma config ALTVREF = ALTVREDIS    // Alternate VREF location Enable->VREF is on a default pin (VREF+ on RA9 and VREF- on RA10)
#pragma config WINDIS = OFF    // Windowed WDT->Standard Watchdog Timer enabled,(Windowed-mode is disabled)
#pragma config FWDTEN = ON    // Watchdog Timer->Watchdog Timer is enabled
#pragma config ICS = PGx1    // Emulator Pin Placement Select bits->Emulator functions are shared with PGEC1/PGED1
#pragma config GWRP = OFF    // General Segment Write Protect->Writes to program memory are allowed
#pragma config GCP = OFF    // General Segment Code Protect->Code protection is disabled
#pragma config JTAGEN = OFF    // JTAG Port Enable->JTAG port is disabled

void OtherTasks(void)
{
    CheckExtInt4();
}

void SYSTEM_Initialize( SYSTEM_STATE state )
{
    PIN_MANAGER_Initialize();
    INTERRUPT_Initialize();
    OSCILLATOR_Initialize();
    EXT_INT_Initialize();
    TIMERS_Initialize();
    InitializeWaveformDACs();
    InitializeWaveformGenerators();
    
    switch(state)
    {
        case SYSTEM_STATE_USB_START:
            //Make sure that the general purpose output driver multiplexed with
            //the VBUS pin is always consistently configured to be tri-stated in
            //USB applications, so as to avoid any possible contention with the host.
            //(ex: maintain TRISFbits.TRISF7 = 1 at all times).
            TRISFbits.TRISF7 = 1;
            break;
            
        case SYSTEM_STATE_USB_SUSPEND:
            //If developing a bus powered USB device that needs to be USB compliant,
            //insert code here to reduce the I/O pin and microcontroller power consumption,
            //so that the total current is <2.5mA from the USB host's VBUS supply.
            //If developing a self powered application (or a bus powered device where
            //official USB compliance isn't critical), nothing strictly needs
            //to be done during USB suspend.

            USBSleepOnSuspend();
            break;

        case SYSTEM_STATE_USB_RESUME:
            break;

        default:
            break;
    }
}

#if defined(USB_INTERRUPT)
void __attribute__((interrupt,auto_psv)) _USB1Interrupt()
{
    USBDeviceTasks();
}
#endif

void PIN_MANAGER_Initialize(void)
{
    LATA = 0x0002;
    LATB = 0x0000;
    LATC = 0x0010;
    LATD = 0xF000;
    LATE = 0x0000;
    LATF = 0x2010;
    LATG = 0x11C0;
    
    TRISA = 0xC000;
    TRISB = 0x0003;
    TRISC = 0x9000;
    TRISD = 0x0000;
    TRISE = 0x0000;
    TRISF = 0x0100;
    TRISG = 0x000C;
    
    CNPD1 = 0xF8FF;
    CNPD2 = 0xFF01;
    CNPD3 = 0xFF83;
    CNPD4 = 0xFDFE;
    CNPD5 = 0x683F;
    CNPD6 = 0x0007;
    CNPU1 = 0x0700;
    CNPU2 = 0x003E;
    CNPU3 = 0x007C;
    CNPU4 = 0x0201;
    CNPU5 = 0x94C0;
    CNPU6 = 0x0000;
    
    ODCA = 0x0000;
    ODCB = 0x0000;
    ODCC = 0x0000;
    ODCD = 0x0000;
    ODCE = 0x0000;
    ODCF = 0x0000;
    ODCG = 0x0000;
    
    ANSA = 0x0000;
    ANSB = 0x0000;
    ANSC = 0x0000;
    ANSD = 0x0000;
    ANSE = 0x0000;
    ANSF = 0x0000;
    ANSG = 0x0000;
    
    __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS
    
    //RA14 = EXT_INT_1
    RPINR0bits.INT1R = 0x0024;
    //RA15 = EXT_INT_2
    RPINR1bits.INT2R = 0x0023;   //RA15->EXT_INT:INT2;
    //RC14 = EXT_INT_3
    RPINR1bits.INT3R = 0x0025;
    //RFx = serial port (3 = SCK, 2 = MOSI, 8 = MISO, 5 = SS)
    RPOR8bits.RP16R = 0x0008;   //RF3->SPI1:SCK1OUT;
    RPOR8bits.RP17R = 0x0009;   //RF5->SPI1:SS1OUT;
    RPOR15bits.RP30R = 0x0007;   //RF2->SPI1:SDO1;
    RPINR20bits.SDI1R = 0x000F;   //RF8->SPI1:SDI1;

    __builtin_write_OSCCONL(OSCCON | 0x40); // lock   PPS

}

void OSCILLATOR_Initialize(void)
{
    // CPDIV 1:1; RCDIV FRC/2; DOZE 1:8; DOZEN disabled; ROI disabled; 
    CLKDIV = 0x3100;
    // TUN Center frequency; 
    OSCTUN = 0x0000;
    // CF no clock failure; NOSC PRIPLL; SOSCEN disabled; POSCEN disabled; CLKLOCK unlocked; OSWEN Switch is Complete; 
    __builtin_write_OSCCONH((uint8_t) ((0x0300 >> _OSCCON_NOSC_POSITION) & 0x00FF));
    __builtin_write_OSCCONL((uint8_t) ((0x0300 | _OSCCON_OSWEN_MASK) & 0xFF));
    // Wait for Clock switch to occur
    while (OSCCONbits.OSWEN != 0);
}

void INTERRUPT_Initialize(void)
{
    //Make sure nested interrupts are enabled
    INTCON1bits.NSTDIS = 0;
}

void EXT_INT_Initialize(void)
{
    //Make sure all interrupts are disabled
    IEC1bits.INT1IE = 0;
    IEC1bits.INT2IE = 0;
    IEC3bits.INT3IE = 0;
    //Clear all the status flags and set the priorities to default (5)
    IFS1bits.INT1IF = 0;
    IFS1bits.INT2IF = 0;
    IFS3bits.INT3IF = 0;
    IPC5bits.INT1IP = 5;
    IPC7bits.INT2IP = 5;
    IPC13bits.INT3IP = 5;
}

void TIMERS_Initialize(void)
{
    IEC0bits.T1IE = 0;
    T1CON = 0x8020;
    IPC0bits.T1IP = 1;
    IFS0bits.T1IF = 0;
    PR1 = 0xFFFF;
    TMR1 = 0;
    
    //Timer 2/3 is 32 bit, longer period (2 us/tic)
    IEC0bits.T3IE = 0;
    T2CON = 0x8008;
    IPC2bits.T3IP = 1;
    IFS0bits.T3IF = 0;
    PR2 = 0x0000;
    PR3 = 0x0080;
    TMR2 = TMR3 = 0;
    
    //Timer 4/5 is 32 bit, shorter period (16 MHz)
    IEC1bits.T5IE = 0;
    T4CON = 0x8008;
    IPC7bits.T5IP = 1;
    IFS1bits.T5IF = 0;
    PR4 = 0x0000;
    PR5 = 0x0040;
    TMR4 = TMR5 = 0;
}