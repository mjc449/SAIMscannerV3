/*******************************************************************************
 * @file dio_pins.c
 * @brief Definitions the primary DIO pins
 *        Includes ISRs associated with trigger pins
 *        Utilities to turn on/off interrupts, adjust function
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

#include "dio_pins.h"

/*******************************************************************************
 * @brief Enable the specified digital pin as an output
 *******************************************************************************/
void DIO_SetPinDirection(DIO_PINS pin, PIN_FUNCTION direction) {
    switch (pin) {
        case PIN_0_0:
            _TRISA2 = direction;
            return;
        case PIN_0_1:
            _TRISA3 = direction;
            return;
        case PIN_0_2:
            _TRISA4 = direction;
            return;
        case PIN_0_3:
            _TRISA5 = direction;
            return;
        case PIN_1_0:
            _TRISA14 = direction;
            return;
        case PIN_1_1:
            _TRISA15 = direction;
            return;
        case PIN_1_2:
            _TRISC13 = direction;
            return;
        case PIN_1_3:
            _TRISC14 = direction;
            return;
        default:
            return;
    }
}

void DIO_WritePin(DIO_PINS pin, PIN_STATE state) {
    switch (pin) {
        case PIN_0_0:
            _LATA2 = state;
            return;
        case PIN_0_1:
            _LATA3 = state;
            return;
        case PIN_0_2:
            _LATA4 = state;
            return;
        case PIN_0_3:
            _LATA5 = state;
            return;
        case PIN_1_0:
            _LATA14 = state;
            return;
        case PIN_1_1:
            _LATA15 = state;
            return;
        case PIN_1_2:
            _LATC13 = state;
            return;
        case PIN_1_3:
            _LATC14 = state;
            return;
        default:
            return;
    }
}

void DIO_TogglePin(DIO_PINS pin) {
    switch (pin) {
        case PIN_0_0:
            _LATA2 = ~_LATA2;
            return;
        case PIN_0_1:
            _LATA3 = ~_LATA3;
            return;
        case PIN_0_2:
            _LATA4 = ~_LATA4;
            return;
        case PIN_0_3:
            _LATA5 = ~_LATA5;
            return;
        case PIN_1_0:
            _LATA14 = ~_LATA14;
            return;
        case PIN_1_1:
            _LATA15 = ~_LATA15;
            return;
        case PIN_1_2:
            _LATC13 = ~_LATC13;
            return;
        case PIN_1_3:
            _LATC14 = ~_LATC14;
            return;
        default:
            return;
    }
}

void DIO_WritePort(DIO_PORT port, uint16_t mask)
{
    switch (port) {
        case DIO_PORT_0:
            LATA = 0x003C & (mask << 2);
            return;
        case DIO_PORT_1:
            LATA = 0xC000 & (mask << 14);
            LATC = 0x6000 & (mask << 12);
            return;
        default:
            return;
    }
}

uint8_t DIO_ReadPort(DIO_PORT port)
{
    switch(port){
        case DIO_PORT_0:
            return ((PORTA & 0x003C) >> 2);
        case DIO_PORT_1:
            return (((PORTA & 0xC000) >> 14) | ((PORTC & 0x6000) >> 12));
        default:
            return;
    }
}