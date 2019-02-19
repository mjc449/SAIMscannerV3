/*******************************************************************************
 * @file timer_callbacks.h
 * @brief Declarations for the timer ISR callback functions
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

#ifndef TIMER_CALLBACKS_H
#define	TIMER_CALLBACKS_H

#include <xc.h>

//Callback list, each function must have an entry
typedef enum {
    DO_NOTHING,
    STROBE_ONE_PIN,
    STROBE_MULTIPLE_PINS
} TIMER_CALLBACK_FUNCTIONS;

/*******************************************************************************
 * Timer callback function prototypes
 ******************************************************************************/
void StrobeOnePinCallback(uint16_t pin);

#endif	/* TIMER_CALLBACKS_H */

