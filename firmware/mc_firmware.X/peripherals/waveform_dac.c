/*******************************************************************************
 * @file waveform_dac.c
 * @brief Definitions for the twin waveform output conditioning DACs
 * 
 * 
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
 ******************************************************************************/


#include "waveform_dac.h"

//Pin-toggle macros for common functions
#define SELECT_X_AMPLITUDE _LATA9 = 1; _LATA10 = 1;
#define SELECT_X_OFFSET _LATA9 = 0; _LATA10 = 0;
#define SELECT_Y_AMPLITUDE _LATA9 = 0; _LATA10 = 0;
#define SELECT_Y_OFFSET _LATA9 = 1; _LATA10 = 1;
#define WRITE_X _LATF14 = 0; _LATF13 = 1;
#define WRITE_Y _LATF4 = 0; _LATF4 = 1;
#define WV_DAC_LOAD _LATF12 = 1; _LATF13 = 0;
#define WV_DAC_RS _LATA1 = 0; _LATA1 = 1;

static uint16_t xOffset, yOffset, xAmplitude, yAmplitude;

void WriteAndLoad(uint16_t xAmp, uint16_t xOff, uint16_t yAmp, uint16_t yOff)
{
    SELECT_X_AMPLITUDE;
    LATB = xAmp;
    WRITE_X;
    SELECT_X_OFFSET;
    LATB = xOff;
    WRITE_X;
    SELECT_Y_AMPLITUDE;
    LATB = yAmp;
    WRITE_Y;
    SELECT_Y_OFFSET;
    LATB = yOff;
    WRITE_Y;
    WV_DAC_LOAD;
}

void InitializeWaveformDACs(void)
{
    WV_DAC_RS;
    SELECT_X_AMPLITUDE;
    LATB = 0x00;
    WRITE_X;
    SELECT_Y_AMPLITUDE;
    LATB = 0x00;
    WRITE_Y;
}

void SetOffsets(uint16_t xVal, uint16_t yVal)
{
    xOffset = xVal;
    yOffset = yVal;
    SELECT_X_OFFSET;
    LATB = xOffset;
    WRITE_X;
    SELECT_Y_OFFSET;
    LATB = yOffset;
    WRITE_Y;
    WV_DAC_LOAD;
}

void SetAmplitudes(uint16_t xVal, uint16_t yVal)
{
    
}