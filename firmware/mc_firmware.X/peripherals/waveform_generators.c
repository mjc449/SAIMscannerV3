/*******************************************************************************
 * @file waveform_generators.h
 * @brief Declarations for the 2 waveform generators
 * 
 * Each case in the switch should be kept succinct.  For commands that require
 * more complex processing add a simple function to assemble the packet into
 * the appropriate data types and calls.
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


#include "waveform_generators.h"
#include <p24FJ256GB210.h>
#include <stdbool.h>

#define WV_X_DATA _LATC4
#define WV_Y_DATA _LATG8
#define WV_SYNC _LATG7
#define WV_MCLK _LATG9
#define WV_SCK_PULSE _LATG6 = 0; _LATG6 = 1;
#define WV_SCK _LATG6


static volatile struct
{
    uint16_t :2;
    uint16_t B28:1;
    uint16_t HLB:1;
    uint16_t FSEL:1;
    uint16_t PSEL:1;
    uint16_t :1;
    uint16_t RS;
    uint16_t MCLKDIS:1;
    uint16_t DACDIS:1;
    uint16_t OPBITEN:1;
    uint16_t :1;
    uint16_t DIV2;
    uint16_t :1;
    uint16_t MODE;
    uint16_t :1;
}XControlWord, YControlWord;

void WriteWord(uint16_t x_msg, uint16_t y_msg)
{
    int i;
    WV_SYNC = 0;
    for(i = 15; i > -1 ; i--)
    {
        WV_SCK = 1;
        WV_X_DATA = (x_msg >> i) & 1;
        WV_Y_DATA = (y_msg >> i) & 1;
        WV_SCK = 0;
    }
    WV_SYNC = 1;
    WV_SCK = 1;
}

void UpdateControlRegs(void)
{
    WV_SYNC = 0;
    WV_X_DATA = 0;
    WV_Y_DATA = 0;
    WV_SCK_PULSE;
    WV_X_DATA = 0;
    WV_Y_DATA = 0;
    WV_SCK_PULSE;
    WV_X_DATA = XControlWord.B28;
    WV_Y_DATA = YControlWord.B28;
    WV_SCK_PULSE;
    WV_X_DATA = XControlWord.HLB;
    WV_Y_DATA = YControlWord.HLB;
    WV_SCK_PULSE;
    WV_X_DATA = XControlWord.FSEL;
    WV_Y_DATA = YControlWord.FSEL;
    WV_SCK_PULSE;
    WV_X_DATA = XControlWord.PSEL;
    WV_Y_DATA = YControlWord.PSEL;
    WV_SCK_PULSE;
    WV_X_DATA = 0;
    WV_Y_DATA = 0;
    WV_SCK_PULSE;
    WV_X_DATA = XControlWord.RS;
    WV_Y_DATA = YControlWord.RS;
    WV_SCK_PULSE;
    WV_X_DATA = XControlWord.MCLKDIS;
    WV_Y_DATA = YControlWord.MCLKDIS;
    WV_SCK_PULSE;
    WV_X_DATA = XControlWord.DACDIS;
    WV_Y_DATA = YControlWord.DACDIS;
    WV_SCK_PULSE;
    WV_X_DATA = XControlWord.OPBITEN;
    WV_Y_DATA = YControlWord.OPBITEN;
    WV_SCK_PULSE;
    WV_X_DATA = 0;
    WV_Y_DATA = 0;
    WV_SCK_PULSE;
    WV_X_DATA = XControlWord.DIV2;
    WV_Y_DATA = YControlWord.DIV2;
    WV_SCK_PULSE;
    WV_X_DATA = 0;
    WV_Y_DATA = 0;
    WV_SCK_PULSE;
    WV_X_DATA = XControlWord.MODE;
    WV_Y_DATA = YControlWord.MODE;
    WV_SCK_PULSE;
    WV_X_DATA = 0;
    WV_Y_DATA = 0;
    WV_SCK = 0;
    WV_SYNC = 1;
    WV_SCK = 1;
}

void InitializeWaveformGenerators(void)
{
    WV_SYNC = 1;
    WV_MCLK = 0;
    //Set the initial values in ControlWord
    XControlWord.B28 = 1;
    XControlWord.HLB = 0;
    XControlWord.FSEL = 0;
    XControlWord.PSEL = 0;
    XControlWord.RS = 1;
    XControlWord.MCLKDIS = 0;
    XControlWord.DACDIS = 0;
    XControlWord.OPBITEN = 0;
    XControlWord.DIV2 = 0;
    XControlWord.MODE = 0;
    
    YControlWord = XControlWord;
    UpdateControlRegs();
    
    SetPhase(PHASE_90);
    SetSameFreq(ONE_KHZ);
    
    YControlWord.RS = XControlWord.RS = 0;
    UpdateControlRegs();
    WV_MCLK = 1;
}

void WaveformOutputEnable(WAVEFORM_AXIS axis, uint8_t onOff)
{
    onOff = (onOff > 0) ? 0 : 1;
    switch(axis)
    {
        case X_AXIS:
            XControlWord.RS = onOff;
            break;
        case Y_AXIS:
            YControlWord.RS = onOff;
            break;
        case BOTH_AXIS:
            XControlWord.RS = YControlWord.RS = onOff;
            break;
        default:
            break;
    }
    UpdateControlRegs();
}

void SetPhase(uint16_t phase)
{
    phase = (phase > 0xFFF) ? phase % 0xFFF : phase;
    phase |= 0xC000;
    WriteWord(0xC000, phase);
}

void SetSameFreq(uint32_t frequency)
{
    int LSB = frequency & 0x3FFF;
    LSB |= 0x4000;
    WriteWord(LSB, LSB);
    int MSB = (frequency >> 14) & 0x3FFF;
    MSB |= 0x4000;
    WriteWord(MSB, MSB);
}

void SetXFreq(uint32_t frequency)
{
    int LSB = frequency & 0x3FFF;
    LSB |= 0x4000;
    WriteWord(LSB, 0x8000);
    int MSB = (frequency >> 14) & 0x3FFF;
    MSB |= 0x4000;
    WriteWord(MSB, 0x8000);
}

void SetYFreq(uint32_t frequency)
{
    int LSB = frequency & 0x3FFF;
    LSB |= 0x4000;
    WriteWord(LSB, 0x8000);
    int MSB = (frequency >> 14) & 0x3FFF;
    MSB |= 0x4000;
    WriteWord(MSB, 0x8000);
}

void SetOutputTypes(WAVEFORM_TYPES xOutput, WAVEFORM_TYPES yOutput)
{
    switch(xOutput)
    {
        case SINE_OUTPUT:
            XControlWord.OPBITEN = 0;
            XControlWord.MODE = 0;
            XControlWord.DACDIS = 0;
            break;
        case TRIANGLE_OUTPUT:
            XControlWord.OPBITEN = 0;
            XControlWord.MODE = 1;
            XControlWord.DACDIS = 0;
            break;
        case SQUARE_OUTPUT:
            XControlWord.OPBITEN = 1;
            XControlWord.MODE = 0;
            XControlWord.DIV2 = 1;
            break;
        case SQUARE_OUTPUT_DIV2:
            XControlWord.OPBITEN = 1;
            XControlWord.MODE = 0;
            XControlWord.DIV2 = 0;
            break;
        default:
            break;
    }
    
    switch(yOutput)
    {
        case SINE_OUTPUT:
            YControlWord.OPBITEN = 0;
            YControlWord.MODE = 0;
            YControlWord.DACDIS = 0;
            break;
        case TRIANGLE_OUTPUT:
            YControlWord.OPBITEN = 0;
            YControlWord.MODE = 1;
            YControlWord.DACDIS = 0;
            break;
        case SQUARE_OUTPUT:
            YControlWord.OPBITEN = 1;
            YControlWord.MODE = 0;
            YControlWord.DIV2 = 1;
            break;
        case SQUARE_OUTPUT_DIV2:
            YControlWord.OPBITEN = 1;
            YControlWord.MODE = 0;
            YControlWord.DIV2 = 0;
            break;
        default:
            break;
    }
    
    UpdateControlRegs();
}