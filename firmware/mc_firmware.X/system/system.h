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

#ifndef SYSTEM_H
#define SYSTEM_H

#include <p24FJ256GB210.h>
#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include "traps.h"


#define _XTAL_FREQ  32000000UL

#define ERROR_HANDLER __attribute__((interrupt, no_auto_psv))
#define ERROR_HANDLER_NORETURN ERROR_HANDLER __attribute__((noreturn))
#define FAILSAFE_STACK_GUARDSIZE 8


/*** System States **************************************************/
typedef enum
{
    SYSTEM_STATE_USB_START,
    SYSTEM_STATE_USB_SUSPEND,
    SYSTEM_STATE_USB_RESUME
} SYSTEM_STATE;

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

void PIN_MANAGER_Initialize(void);
void OSCILLATOR_Initialize(void);
void EXT_INT_Initialize(void);
void INTERRUPT_Initialize(void);
void TIMERS_Initialize(void);

/*******************************************************************************
 *@brief Other system tasks that are executed by polling
 *
 * Called once per main loop, here is where any non-interrupt status checks and
 * periodic housekeeping goes 
 ******************************************************************************/
void OtherTasks(void);

#endif //SYSTEM_H
