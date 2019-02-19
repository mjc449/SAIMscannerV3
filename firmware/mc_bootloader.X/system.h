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

#ifndef SYSTEM_H
#define SYSTEM_H

#include <p24FJ256GB210.h>
#include <stdbool.h>
#include <stdint.h>

#define _XTAL_FREQ 32000000UL
#define MAIN_RETURN int

#define LED_USB_DEVICE_STATE                    _LATA2

/*** System States **************************************************/
typedef enum
{
    SYSTEM_STATE_USB_START,
    SYSTEM_STATE_USB_SUSPEND,
    SYSTEM_STATE_USB_RESUME
} SYSTEM_STATE;

typedef enum 
{
    /* ----- Traps ----- */
    TRAPS_OSC_FAIL = 0, /** Oscillator Fail Trap vector */
    TRAPS_STACK_ERR = 1, /** Stack Error Trap Vector */
    TRAPS_ADDRESS_ERR = 2, /** Address Error Trap Vector */
    TRAPS_MATH_ERR = 3, /** Math Error Trap Vector */
} TRAPS_ERROR_CODE;

void __attribute__((naked, noreturn, weak)) TRAPS_halt_on_error(uint16_t code);

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
void SYSTEM_Initialize( SYSTEM_STATE state );

void OSCILLATOR_Initialize(void);

void PIN_MANAGER_Initialize(void);

void INTERRUPT_Initialize(void);


/*********************************************************************
* Function: void SYSTEM_Tasks(void)
*
* Overview: Runs system level tasks that keep the system running
*
* PreCondition: System has been initalized with SYSTEM_Initialize()
*
* Input: None
*
* Output: None
*
********************************************************************/
//void SYSTEM_Tasks(void);
#define SYSTEM_Tasks()

#endif //SYSTEM_H
