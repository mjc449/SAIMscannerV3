/*******************************************************************************
 * @file dio_interrupts.c
 * @brief Definitions for the external interrupt functions
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

#include <p24FJ256GB210.h>
#include "ext_interrupts.h"
#include "ext_callbacks.h"

static volatile struct ExtIntParameters
{
    EXT_CALLBACK_FUNCTIONS callback;
    INT_POLARITY polarity;
    uint8_t priority;
    uint16_t var0;
    uint16_t var1;
}EXT_INT_1_PARAMS, EXT_INT_2_PARAMS, EXT_INT_3_PARAMS, EXT_INT_4_PARAMS;

static volatile INT_STATE extInt4OnOff;

static inline void EXT_INT_4_ENABLE(INT_STATE state){extInt4OnOff = state;}

/*******************************************************************************
 * External trigger callables
 ******************************************************************************/
void EnableDisableTrigger(EXT_INT_LIST trigger, INT_STATE state, INT_POLARITY polarity, uint8_t priority)
{
    switch(trigger)
    {
        case INT_1:
            _TRISA14 = 1;
            EXT_INT_1_ENABLE(state);
            EXT_INT_1_PRIORITY(priority);
            EXT_INT_1_PARAMS.priority = priority;
            EXT_INT_1_POLARITY(polarity);
            EXT_INT_1_PARAMS.polarity = polarity;
            INTCON2bits.INT1EP = ((polarity % 2) == 0) ? 0 : 1;
            break;
        case INT_2:
            _TRISA15 = 1;
            EXT_INT_2_ENABLE(state);
            EXT_INT_2_PRIORITY(priority);
            EXT_INT_2_PARAMS.priority = priority;
            EXT_INT_2_POLARITY(polarity);
            EXT_INT_2_PARAMS.polarity = polarity;
            INTCON2bits.INT2EP = ((polarity % 2) == 0) ? 0 : 1;
            break;
        case INT_3:
            _TRISC14 = 1;
            EXT_INT_3_ENABLE(state);
            EXT_INT_3_PRIORITY(priority);
            EXT_INT_3_PARAMS.priority = priority;
            EXT_INT_3_POLARITY(polarity);
            EXT_INT_3_PARAMS.polarity = polarity;
            INTCON2bits.INT3EP = ((polarity % 2) == 0) ? 0 : 1;
            break;
        case INT_4:
            _TRISC13 = 1;
            EXT_INT_4_ENABLE(state);
            EXT_INT_4_PARAMS.polarity = polarity;
            break;
        default:
            break;
    }
    return;
}

void TogglePinsOnTrigger(EXT_INT_LIST trigger, uint8_t pinmask)
{
    switch(trigger)
    {
        case INT_1:
            EXT_INT_1_PARAMS.callback = EXT_TOGGLE_PINS_CALLBACK;
            EXT_INT_1_PARAMS.var0 = (uint16_t)pinmask;
            break;
        case INT_2:
            EXT_INT_1_PARAMS.callback = EXT_TOGGLE_PINS_CALLBACK;
            EXT_INT_2_PARAMS.var0 = (uint16_t)pinmask;
            break;
        case INT_3:
            EXT_INT_3_PARAMS.callback = EXT_TOGGLE_PINS_CALLBACK;
            EXT_INT_3_PARAMS.var0 = (uint16_t)pinmask;
            break;
        case INT_4:
            EXT_INT_4_PARAMS.callback = EXT_TOGGLE_PINS_CALLBACK;
            EXT_INT_2_PARAMS.var0 = (uint16_t)pinmask;
            break;
        default:
            break;
    }
    return;
}

void EnableDisableTimerOnTrigger(EXT_INT_LIST trigger, TIMER_LIST timer, bool onOff)
{
    
}

void ToggleTimerOnTrigger(EXT_INT_LIST trigger, TIMER_LIST timer)
{
    switch(trigger)
    {
        case INT_1:
            EXT_INT_1_PARAMS.callback = EXT_TOGGLE_TIMER_CALLBACK;
            EXT_INT_1_PARAMS.var0 = timer;
            break;
        case INT_2:
            EXT_INT_2_PARAMS.callback = EXT_TOGGLE_TIMER_CALLBACK;
            EXT_INT_2_PARAMS.var0 = timer;
            break;
        case INT_3:
            EXT_INT_3_PARAMS.callback = EXT_TOGGLE_TIMER_CALLBACK;
            EXT_INT_3_PARAMS.var0 = timer;
            break;
        case INT_4:
            EXT_INT_4_PARAMS.callback = EXT_TOGGLE_TIMER_CALLBACK;
            EXT_INT_4_PARAMS.var0 = timer;
            break;
        default:
            break;
    }
    return;
}
/*******************************************************************************
 * External interrupt ISRs
 ******************************************************************************/
void __attribute__((interrupt, no_auto_psv)) _INT1Interrupt(void)
{
    switch(EXT_INT_1_PARAMS.callback)
    {
        case EXT_TOGGLE_PINS_CALLBACK:
            TogglePinsOnTriggerCallback(EXT_INT_1_PARAMS.var0);
            break;
        case EXT_START_TIMER_CALLBACK:
            EnableDisableTimer(EXT_INT_1_PARAMS.var0, true, EXT_INT_1_PARAMS.priority);
            break;
        case EXT_STOP_TIMER_CALLBACK:
            EnableDisableTimer(EXT_INT_1_PARAMS.var0, false, EXT_INT_1_PARAMS.priority);
            break;
        case EXT_TOGGLE_TIMER_CALLBACK:
            ToggleTimerOnTriggerCallback(EXT_INT_1_PARAMS.var0, EXT_INT_1_PARAMS.priority);
            break;
            
        default:
            break;
    }
    if(EXT_INT_1_PARAMS.polarity == 2)
        INTCON2bits.INT1EP = ~INTCON2bits.INT1EP;
    IFS1bits.INT1IF = 0;
    return;
}

void __attribute__((interrupt, no_auto_psv)) _INT2Interrupt(void)
{
    switch(EXT_INT_2_PARAMS.callback)
    {
        case EXT_TOGGLE_PINS_CALLBACK:
            TogglePinsOnTriggerCallback(EXT_INT_2_PARAMS.var0);
            break;
        case EXT_START_TIMER_CALLBACK:
            EnableDisableTimer(EXT_INT_2_PARAMS.var0, true, EXT_INT_2_PARAMS.priority);
            break;
        case EXT_STOP_TIMER_CALLBACK:
            EnableDisableTimer(EXT_INT_2_PARAMS.var0, false, EXT_INT_2_PARAMS.priority);
            break;
        case EXT_TOGGLE_TIMER_CALLBACK:
            ToggleTimerOnTriggerCallback(EXT_INT_2_PARAMS.var0, EXT_INT_2_PARAMS.priority);
            break;
            
        default:
            break;
    }
    if(EXT_INT_2_PARAMS.polarity == 2)
        INTCON2bits.INT2EP = ~INTCON2bits.INT2EP;
    IFS1bits.INT2IF = 0;
    return;
}

void __attribute__((interrupt, no_auto_psv)) _INT3Interrupt(void)
{
    switch(EXT_INT_3_PARAMS.callback)
    {
        case EXT_TOGGLE_PINS_CALLBACK:
            TogglePinsOnTriggerCallback(EXT_INT_3_PARAMS.var0);
            break;
        case EXT_START_TIMER_CALLBACK:
            EnableDisableTimer(EXT_INT_3_PARAMS.var0, true, EXT_INT_3_PARAMS.priority);
            break;
        case EXT_STOP_TIMER_CALLBACK:
            EnableDisableTimer(EXT_INT_3_PARAMS.var0, false, EXT_INT_3_PARAMS.priority);
            break;
        case EXT_TOGGLE_TIMER_CALLBACK:
            ToggleTimerOnTriggerCallback(EXT_INT_3_PARAMS.var0, EXT_INT_3_PARAMS.priority);
            break;
            
        default:
            break;
    }
    IFS3bits.INT3IF = 0;
    return;
}

//Not a true ISR, this "interrupt" is really just pin polling
void CheckExtInt4(void)
{
    if(!extInt4OnOff)
        return;
    static int previousState;
    //When the pin value has changed
    if(_RC13 != previousState)
    {
        //If the pin state matches the transition type exit
        if(_RC13 == EXT_INT_4_PARAMS.polarity)
        {
            previousState = _RC13;
            return;
        }
        //Otherwise service the request
        switch(EXT_INT_4_PARAMS.callback)
        {
            
            default:
                break;
        }
        previousState = _RC13;
    }
    return;
}