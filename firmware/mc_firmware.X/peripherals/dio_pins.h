/*******************************************************************************
 * @file dio_pins.h
 * @brief Definitions for the DIO pins, 3 of which can be interrupt
 *        triggers.
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

#ifndef DIO_PINS_H
#define	DIO_PINS_H

#include <xc.h>

typedef enum
{
    DIO_PORT_0 = 0,
    DIO_PORT_1 = 1
} DIO_PORT;

typedef enum
{
    PIN_INPUT = 1,
    PIN_OUTPUT =0
} PIN_FUNCTION;

typedef enum
{
    PIN_HIGH = 1,
    PIN_LOW = 0
} PIN_STATE;

typedef enum
{
    PIN_0_0 = 0x00,
    PIN_0_1 = 0x01,
    PIN_0_2 = 0x02,
    PIN_0_3 = 0x03,
    PIN_1_0 = 0x04,
    PIN_1_1 = 0x05,
    PIN_1_2 = 0x06,
    PIN_1_3 = 0x07
} DIO_PINS;

void DIO_SetPinDirection(DIO_PINS, PIN_FUNCTION);

void DIO_WritePin(DIO_PINS, PIN_STATE);

void DIO_TogglePin(DIO_PINS);

void DIO_WritePort(DIO_PORT, uint16_t);

uint8_t DIO_ReadPort(DIO_PORT);

#endif	/* DIO_PINS_H */

