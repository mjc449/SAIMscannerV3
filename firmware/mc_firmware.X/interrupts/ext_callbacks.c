/*******************************************************************************
 * @file ext_callbacks.c
 * @brief Definitions for the external interrupt callback routines.
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

#include "ext_callbacks.h"
#include "timer_interrupts.h"


void TogglePinsOnTriggerCallback(uint8_t pins)
{
    int i = 0;
    for(i = 0; i < 8; i++)
        if(0x01 & (pins >> i))
            DIO_TogglePin(i);
    return;
}

void ToggleTimerOnTriggerCallback(TIMER_LIST timer, uint8_t priority)
{
    bool onOff = false;
    switch(timer)
    {
        case TIMER_0:
            onOff = ~IEC0bits.T1IE;
            break;
        case TIMER_1:
            onOff = ~IEC0bits.T3IE;
            break;
        case TIMER_2:
            onOff = ~IEC1bits.T5IE;
            break;
        default:
            break;
    }
    EnableDisableTimer(timer, onOff, priority); 
    return;
}