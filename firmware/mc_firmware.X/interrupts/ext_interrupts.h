/*******************************************************************************
 * @file dio_interrupts.h
 * @brief Declarations for the external hardware interrupts
 * @author Marshall Colville (mjc449@cornell.edu)
 * 
 *  * Copyright 2018 Marshall Colville (mjc449@cornell.edu)
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

#ifndef DIO_INTERRUPTS_H
#define	DIO_INTERRUPTS_H

#include <xc.h>
#include <stdbool.h>
#include "timer_interrupts.h"

#define EXT_INT_1_ENABLE(x)          (IEC1bits.INT1IE = x)
#define EXT_INT_1_PRIORITY(x) (IPC5bits.INT1IP = x)
#define EXT_INT_1_POLARITY(x) (INTCON2bits.INT1EP = x)

#define EXT_INT_2_ENABLE(x)          (IEC1bits.INT2IE = x)
#define EXT_INT_2_PRIORITY(x) (IPC7bits.INT2IP = x)
#define EXT_INT_2_POLARITY(x) (INTCON2bits.INT2EP = x)

#define EXT_INT_3_ENABLE(x)          (IEC3bits.INT3IE = x)
#define EXT_INT_3_PRIORITY(x) (IPC13bits.INT3IP = x)
#define EXT_INT_3_POLARITY(x) (INTCON2bits.INT3EP = x)


typedef enum {
    INT_1 = 0,
    INT_2 = 1,
    INT_3 = 2,
    INT_4 = 3
} EXT_INT_LIST;

typedef enum {
    DISABLED = 0,
    ENABLED = 1
} INT_STATE;

typedef enum {
    RISING_EDGE = 0,
    FALLING_EDGE = 1,
    BOTH_EDGE = 2
} INT_POLARITY;

/*******************************************************************************
 * @brief Check the status of RC13 and run the callback if needed
 * 
 * Not a true interrupt, but an execution branch on change of state to simulate
 * an additional external interrupt.  Called periodically in the main loop.
 ******************************************************************************/
void CheckExtInt4(void);

/*******************************************************************************
 * @brief Turn one of the trigger sources on or off
 ******************************************************************************/
void EnableDisableTrigger(EXT_INT_LIST trigger, INT_STATE state, INT_POLARITY polarity, uint8_t priority);

/*******************************************************************************
 * @brief Toggles the state of the pins in pinmask on each interrupt
 * 
 * @param trigger = ext int to use
 * @param pins = 8-bit pinmask of the pins to toggle
 ******************************************************************************/
void TogglePinsOnTrigger(EXT_INT_LIST trigger, uint8_t pinmask);

/*******************************************************************************
 * @brief Start or stop a timer interrupt on a trigger signal
 * 
 * The timer interrupt callback must be set prior to calling this function
 * The newly started timer will have the same priority as the calling ext int
 * 
 * @param trigger = ext int to trigger
 * @param timer = timer interrupt to start
 ******************************************************************************/
void EnableDisableTimerOnTrigger(EXT_INT_LIST trigger, TIMER_LIST timer, bool onOff);

/*******************************************************************************
 * @brief Toggles the state of a timer on each interrupt
 * 
 * The timer interrupt callback must be setup prior to calling this function
 * The timer will have the same priority as the trigger
 * 
 * @param trigger = trigger
 * @param timer  = timer
 ******************************************************************************/
void ToggleTimerOnTrigger(EXT_INT_LIST trigger, TIMER_LIST timer);

#endif //DIO_INTERRUPTS_H