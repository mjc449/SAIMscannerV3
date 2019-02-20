
/*******************************************************************************
 * @file timers.c
 * @brief Definitions for the timers
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


#include "timer_interrupts.h"
#include "timer_callbacks.h"


static volatile struct TimerParameters
{
    uint32_t period;
    TIMER_CALLBACK_FUNCTIONS callBack;
    uint16_t count;
    uint16_t stop;
    uint16_t var0;
    uint16_t var1;
    uint16_t var2;
} TIMER_0_PARAMS, TIMER_1_PARAMS, TIMER_2_PARAMS;



/*******************************************************************************
 * Timer interface functions
 ******************************************************************************/
void EnableDisableTimer(TIMER_LIST timer, bool onOff, uint8_t priority)
{
    switch(timer)
    {
        case TIMER_0:
            IFS0bits.T1IF = 0;
            if(onOff)
            {
                PR1 = (uint16_t)(TIMER_0_PARAMS.period);
                TMR1 = 0;
                IPC0bits.T1IP = priority;
                ENABLE_TIMER_1();
            }
            else
                DISABLE_TIMER_1();
            break;
        case TIMER_1:
            IFS0bits.T3IF = 0;
            if(onOff)
            {
                PR2 = (uint16_t)(TIMER_1_PARAMS.period);
                PR3 = (uint16_t)(TIMER_1_PARAMS.period >> 16);
                TMR2 = TMR3 = 0;
                IPC2bits.T3IP = priority;
                ENABLE_TIMER_2();
            }
            else
                DISABLE_TIMER_2();
            break;
            IFS1bits.T5IF = 0;
        case TIMER_2:
            if(onOff)
            {
                PR4 = (uint16_t)(TIMER_2_PARAMS.period);
                PR5 = (uint16_t)(TIMER_2_PARAMS.period >> 16);
                TMR4 = TMR5 = 0;
                IPC7bits.T5IP = priority;
                ENABLE_TIMER_3();
            }
            else
                DISABLE_TIMER_3();
            break;
    }
}

void StrobeOnePin(DIO_PINS pin, TIMER_LIST timer, uint32_t period, uint16_t cycles)
{
    switch(timer)
    {
        case TIMER_0:
            IEC0bits.T1IE = 0;
            TIMER_0_PARAMS.period = period;
            TIMER_0_PARAMS.callBack = STROBE_ONE_PIN;
            TIMER_0_PARAMS.stop = cycles;
            TIMER_0_PARAMS.count = 0;
            TIMER_0_PARAMS.var0 = pin;
            return;
        case TIMER_1:
            IEC0bits.T3IE = 0;
            TIMER_1_PARAMS.period = period;
            TIMER_1_PARAMS.callBack = STROBE_ONE_PIN;
            TIMER_1_PARAMS.stop = cycles;
            TIMER_1_PARAMS.count = 0;
            TIMER_1_PARAMS.var0 = pin;
            return;
        case TIMER_2:
            IEC1bits.T5IE = 0;
            TIMER_2_PARAMS.period = period;
            TIMER_2_PARAMS.callBack = STROBE_ONE_PIN;
            TIMER_2_PARAMS.stop = cycles;
            TIMER_2_PARAMS.count = 0;
            TIMER_2_PARAMS.var0 = pin;
            return;
        default:
            return;
    }
}


/*******************************************************************************
 * Timer ISRs
 ******************************************************************************/
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt()
{
    //call the appropriate callback
    switch(TIMER_0_PARAMS.callBack)
    {
        case STROBE_ONE_PIN:
            StrobeOnePinCallback(TIMER_0_PARAMS.var0);
            break;
        default:
            break;
    }
    //if the number of 
    if(++TIMER_0_PARAMS.count > TIMER_0_PARAMS.stop)
    {
        IEC0bits.T1IE = 0;
    }
    IFS0bits.T1IF = 0;
    return;
}

void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void)
{
    switch(TIMER_1_PARAMS.callBack)
    {
        case STROBE_ONE_PIN:
            StrobeOnePinCallback(TIMER_1_PARAMS.var0);
            break;
        default:
            break;
    }
    if(++TIMER_1_PARAMS.count > TIMER_1_PARAMS.stop)
    {
        IEC0bits.T3IE = 0;
    }
    IFS0bits.T3IF = 0;
    return;
}

void __attribute__((interrupt, no_auto_psv)) _T5Interrupt()
{
    switch(TIMER_2_PARAMS.callBack)
    {
        case STROBE_ONE_PIN:
            StrobeOnePinCallback(TIMER_2_PARAMS.var0);
            break;
        default:
            break;
    }
    if(++TIMER_2_PARAMS.count > TIMER_2_PARAMS.stop)
    {
        IEC1bits.T5IE = 0;
    }
    IFS1bits.T5IF = 0;
    return;
}
