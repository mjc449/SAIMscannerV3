/*******************************************************************************
 * @file ext_callbacks.h
 * @brief Declarations and types for the external interrupt callback routines.
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

#ifndef EXT_CALLBACKS_H
#define	EXT_CALLBACKS_H


#include <xc.h>
#include "../peripherals/dio_pins.h"
#include "timer_interrupts.h"

typedef enum {
    EXT_TOGGLE_PINS_CALLBACK,
    EXT_START_TIMER_CALLBACK,
    EXT_STOP_TIMER_CALLBACK,
    EXT_TOGGLE_TIMER_CALLBACK
} EXT_CALLBACK_FUNCTIONS;

/*******************************************************************************
 * @brief Toggle an output pin state on each interrupt.
 * 
 * @param pins = dio pin mask to toggle, pins must be set to output externally
 ******************************************************************************/
void TogglePinsOnTriggerCallback(uint8_t pins);

/*******************************************************************************
 * @brief Start one of the timers on an external signal.
 * 
 * The timer's function and parameters need to be initialized first.
 * 
 * @param timer = which timer to use
 * @param priority = timer interrupt priority
 ******************************************************************************/
void ToggleTimerOnTriggerCallback(TIMER_LIST timer, uint8_t priority);


#endif	/* EXT_CALLBACKS_H */

