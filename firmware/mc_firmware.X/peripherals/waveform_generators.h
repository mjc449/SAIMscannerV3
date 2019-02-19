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


#ifndef WAVEFORM_GENERATORS_H
#define	WAVEFORM_GENERATORS_H

#include <xc.h>

#define PHASE_90 0x0400
#define ONE_KHZ 0x000029F1

typedef enum {
    SINE_OUTPUT = 0,
    TRIANGLE_OUTPUT = 1,
    SQUARE_OUTPUT = 2,
    SQUARE_OUTPUT_DIV2 = 3
} WAVEFORM_TYPES;

typedef enum {
    X_AXIS = 0,
    Y_AXIS = 1,
    BOTH_AXIS = 2
} WAVEFORM_AXIS;

/*******************************************************************************
 * @brief Initializes the waveform generators to 1 kHz sine output
 * 
 * Should be called on startup.  The function generators are set to low-word
 * only frequency range, sine output @1 kHz, pi/4 phase.
 ******************************************************************************/
void InitializeWaveformGenerators(void);

/*******************************************************************************
 * @brief Turns the waveform output on or off
 * 
 * When the waveform is shutoff the output is midrange DAC (0V in bipolar mode)
 * 
 * @param axis = axis to turn on or off
 * @param onOff = output state
 ******************************************************************************/
void WaveformOutputEnable(WAVEFORM_AXIS axis, uint8_t onOff);

/*******************************************************************************
 * @brief Set the phase between x and y outputs
 * 
 * Phase values are entered as y wrt to x and run from 0 to 4096 (0 - 2*pi)
 * 
 * @param phase = offset to be applied to the y output
 ******************************************************************************/
void SetPhase(uint16_t phase);

/*******************************************************************************
 * @brief Write the same frequency to both outputs
 * 
 * The frequency register is split into upper and lower 14 bit words.
 * This function takes a 28 bit frequency word and writes the proper registers
 * 
 * @param frequency = 28 bit frequency value (0.1 to 25 mHz)
 ******************************************************************************/
void SetSameFreq(uint32_t frequency);

/*******************************************************************************
 * @brief Write a new frequency value to one or the other outputs
 * 
 * The frequency register is split into upper and lower 14 bit words.
 * This function takes a 28 bit frequency word and writes the proper registers
 * 
 * @param frequency = 28 bit frequency value (0.1 to 25 mHz)
 ******************************************************************************/
void SetXFreq(uint32_t frequency);
void SetYFreq(uint32_t frequency);

/*******************************************************************************
 * @brief Change the output type for each axis
 * 
 * @param xOutput
 * @param yOutput
 ******************************************************************************/
void SetOutputTypes(WAVEFORM_TYPES xOutput, WAVEFORM_TYPES yOutput);

#endif	/* WAVEFORM_GENERATORS_H */

