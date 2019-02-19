/*******************************************************************************
 * @file command_defines.h
 * @brief Master list of USB commands
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

#ifndef OMB_COMMAND_TYPES_H
#define	OMB_COMMAND_TYPES_H

typedef enum {
    /***************************************************************************
     * 0x0X = Timer functions
     **************************************************************************/
    //Turn a timer on or off
    COMMAND_TIMER_ON_OFF = 0x00,
    //Start a timer based pin toggle
    COMMAND_STROBE_PIN = 0x01,

    /***************************************************************************
     * 0x1X = DIO functions
     **************************************************************************/
    //Make a DIO pin input(1) or output(0)
    COMMAND_SET_PIN_DIRECTION = 0x10,
    //Toggle an output pin
    COMMAND_TOGGLE_PIN = 0x11,
    //Turn a pin on(1) or off (0)
    COMMAND_WRITE_PIN = 0x12,
    //Write all 4 pins in a DIO port
    COMMAND_WRITE_PORT = 0x13,
    //Read the pins in a DIO port
    COMMAND_READ_PORT = 0x14,

    /***************************************************************************
     * 0x2X = External trigger functions
     **************************************************************************/
    //Enable or disable a trigger
    COMMAND_TRIGGER_ON_OFF = 0x20,
    //Toggles the state of the pins on each interrupt
    COMMAND_TOGGLE_PINS_ON_TRIGGER = 0x21,
    //Start timer on interrupt from trigger
    COMMAND_START_TIMER_ON_TRIGGER = 0x22,
    //Stops timer on interrupt from trigger
    COMMAND_STOP_TIMER_ON_TRIGGER = 0x23,
    //Toggles a timer interrupt on ext
    COMMAND_TOGGLE_TIMER_ON_TRIGGER = 0x24,

    /***************************************************************************
     * 0x3X = Waveform generator commands
     **************************************************************************/
    //Disable or enable the output
    COMMAND_WAVEFORM_ON_OFF = 0x30,
    //Change the output frequency
    COMMAND_WAVEFORM_FREQUENCY = 0x31,
    //Change the phase between x and y outputs
    COMMAND_WAVEFORM_PHASE = 0x32,
    //Change the output types
    COMMAND_WAVEFORM_OUTPUT_TYPE = 0x33,
    //Set the x-axis frequency
    COMMAND_WAVEFORM_X_FREQUENCY = 0x34,
    //SET the y-axis frequency
    COMMAND_WAVEFORM_Y_FREQUENCY = 0x35,

    /***************************************************************************
     * 0xFX = Special functions
     **************************************************************************/
    //Toggle the power LED (DIO1_0) pin
    COMMAND_IDENTIFY_DEVICE = 0xF0,
    //Read the gotobootloader key and report
    COMMAND_READ_BOOTLOADER_KEY = 0xFD,
    //Sets the goto bootloader flag and performs a soft reset
    COMMAND_GOTO_BOOTLOADER = 0xFE,
    //Soft reset
    COMMAND_RESET_DEVICE = 0xFF
} OMB_COMMAND_LIST;

#endif	/* OMB_COMMAND_TYPES_H */

