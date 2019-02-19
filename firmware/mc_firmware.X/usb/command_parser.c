/*******************************************************************************
 * @file command_parser.c
 * @brief Declarations for the external hardware interrupts
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

#include <xc.h>
#include "command_parser.h"
#include "command_types.h"
#include "peripherals/waveform_generators.h"

static inline uint16_t Make16(uint8_t hByte, uint8_t lByte) {
    uint16_t retval = 0;
    retval = ((uint16_t) hByte << 8) | lByte;
    return retval;
}

static inline uint32_t Make32(uint8_t hWord_hByte, uint8_t hWord_lByte, uint8_t lWord_hByte, uint8_t lWord_lByte) {
    uint32_t retval = ((uint32_t) hWord_hByte << 24)
            | ((uint32_t) hWord_lByte << 16)
            | ((uint32_t) lWord_hByte << 8)
            | lWord_lByte;
    return retval;
}

extern unsigned char ReceivedDataBuffer[64];
extern unsigned char ToSendDataBuffer[64];
uint8_t *dataIn = &ReceivedDataBuffer[1];
uint8_t *dataOut = &ToSendDataBuffer[1];

void SetBootloaderFlag(void);
void ReadBootloaderFlag(uint8_t *);

void ParsePacketIn(void) {
    //Disable user interrupts before processing the packet
    SET_CPU_IPL(7);
    //Storage for return (error) codes, not all cases need returns
    uint8_t response = 0;

    switch (ReceivedDataBuffer[0]) {
            /*******************************************************************
             * Timer functions
             ******************************************************************/
        case COMMAND_TIMER_ON_OFF:
            EnableDisableTimer(dataIn[0], dataIn[1], dataIn[2]);
            break;

        case COMMAND_STROBE_PIN:
            StrobeOnePin(dataIn[0], dataIn[1], Make32(dataIn[2], dataIn[3], dataIn[4], dataIn[5]), Make16(dataIn[6], dataIn[7]));
            break;

            /*******************************************************************
             * DIO functions
             ******************************************************************/
        case COMMAND_SET_PIN_DIRECTION:
            DIO_SetPinDirection(dataIn[0], dataIn[1]);
            break;

        case COMMAND_TOGGLE_PIN:
            DIO_TogglePin(dataIn[0]);
            break;

        case COMMAND_WRITE_PIN:
            DIO_WritePin(dataIn[0], dataIn[1]);
            break;

        case COMMAND_WRITE_PORT:
            DIO_WritePort(dataIn[0], dataIn[1]);
            break;

        case COMMAND_READ_PORT:
            dataOut[0] = DIO_ReadPort(dataIn[0]);
            break;

            /*******************************************************************
             * External trigger functions
             ******************************************************************/
        case COMMAND_TRIGGER_ON_OFF:
            EnableDisableTrigger(dataIn[0], dataIn[1], dataIn[2], dataIn[3]);
            break;
        case COMMAND_TOGGLE_PINS_ON_TRIGGER:
            TogglePinsOnTrigger(dataIn[0], dataIn[1]);
            break;
        case COMMAND_START_TIMER_ON_TRIGGER:
            break;
        case COMMAND_STOP_TIMER_ON_TRIGGER:
            break;
        case COMMAND_TOGGLE_TIMER_ON_TRIGGER:
            ToggleTimerOnTrigger(dataIn[0], dataIn[1]);
            
            /*******************************************************************
             * Waveform functions
             ******************************************************************/
        case COMMAND_WAVEFORM_ON_OFF:
            WaveformOutputEnable(dataIn[0], dataIn[1]);
            break;
        case COMMAND_WAVEFORM_FREQUENCY:
            SetSameFreq(Make32(dataIn[0], dataIn[1], dataIn[2], dataIn[3]));
            break;
        case COMMAND_WAVEFORM_PHASE:
            SetPhase(Make16(dataIn[0], dataIn[1]));
            break;
        case COMMAND_WAVEFORM_OUTPUT_TYPE:
            SetOutputTypes(dataIn[0], dataIn[1]);
            break;
        case COMMAND_WAVEFORM_X_FREQUENCY:
            SetXFreq(Make32(dataIn[0], dataIn[1], dataIn[2], dataIn[3]));
            break;
        case COMMAND_WAVEFORM_Y_FREQUENCY:
            SetYFreq(Make32(dataIn[0], dataIn[1], dataIn[2], dataIn[3]));
            break;
            
            /*******************************************************************
             * Special Functions
             ******************************************************************/
        case COMMAND_IDENTIFY_DEVICE:
            dataOut[0] = 1;
            break;

        case COMMAND_READ_BOOTLOADER_KEY:
            ReadBootloaderFlag(&dataOut[0]);
            break;
        case COMMAND_GOTO_BOOTLOADER:
            SetBootloaderFlag();
            break;
        case COMMAND_RESET_DEVICE:
            __asm__ volatile("reset");
            break;

        default:
            break;
    }
    ToSendDataBuffer[0] = response;
    //Keep the CPU priority low when not doing parsing a command
    SET_CPU_IPL(3);
}

/**Sets the last word in user program memory to 0xFFFFFE, which is the return to
 bootloader code.  On the next reboot the device will enter the bootloader which
 is responsible for clearing the code to return  to normal operation*/
void SetBootloaderFlag(void) {
    //The last address in the user program memory is where we'll store the flag
    uint16_t memOffset = 0x0000;
    uint32_t progMemAddress = 0x2A7FE;
    //Set the very last bit to 0
    uint16_t blFlagL = 0xFFFE;
    uint8_t blFlagH = 0xFF;
    //Initialize word programming and memory pointer
    NVMCON = 0x4003;
    TBLPAG = progMemAddress >> 16;
    memOffset = progMemAddress & 0xFFFF;

    //Write to the latches
    __builtin_tblwtl(memOffset, blFlagL);
    __builtin_tblwth(memOffset, blFlagH);
    __builtin_disi(5);
    __builtin_write_NVM();
    while (NVMCONbits.WR);
    __asm__ volatile("reset");
}

//Simply read the bootloader key and return the lower word

void ReadBootloaderFlag(uint8_t *dst) {
    unsigned int keyVal = 0xFFFF;
    TBLPAG = 0x0002;
    uint16_t offset = 0xA7FE;
    keyVal = __builtin_tblrdl(offset);
    dst[0] = keyVal >> 8;
    dst[1] = keyVal & 0xFF;
    return;
}