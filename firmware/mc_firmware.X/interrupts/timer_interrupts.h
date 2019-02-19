/*******************************************************************************
 * @file timers.h
 * @brief Declarations for the native pic timers and associated ISRs
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

#ifndef TIMER_INTERRUPTS_H
#define	TIMER_INTERRUPTS_H

#include <stdbool.h>
#include <xc.h>
#include "../peripherals/dio_pins.h"

#define ENABLE_TIMER_1()  (IEC0bits.T1IE = 1)
#define ENABLE_TIMER_2()  (IEC0bits.T3IE = 1)
#define ENABLE_TIMER_3()  (IEC1bits.T5IE = 1)
#define DISABLE_TIMER_1() (IEC0bits.T1IE = 0)
#define DISABLE_TIMER_2() (IEC0bits.T3IE = 0)
#define DISABLE_TIMER_3() (IEC1bits.T5IE = 0)

typedef enum
{
    TIMER_0,
    TIMER_1,
    TIMER_2
} TIMER_LIST;

/*******************************************************************************
 * @brief Enable or disable the specified timer 
 * 
 * @param timer = timer to turn on or off
 * @param func = on/off
 ******************************************************************************/
void EnableDisableTimer(TIMER_LIST timer, bool onOff, uint8_t priority);

/*******************************************************************************
 * @brief Set a timer to turn a pin on and off
 * 
 * @param pin = pin to toggle
 * @param timer = timer to use
 * @param period = time between toggles (1/2 period)
 * @param cycles = 2 * number of on/off cycles (0xFFFF for inf)
 ******************************************************************************/
void StrobeOnePin(DIO_PINS, TIMER_LIST, uint32_t period, uint16_t cycles);


#endif	/* TIMER_INTERRUPTS_H */

