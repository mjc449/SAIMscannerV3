/*******************************************************************************
 * @file waveform_dac.h
 * @brief Declarations for the twin 16 bit waveform conditioning DACs
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


#ifndef WAVEFORM_DAC_H
#define	WAVEFORM_DAC_H

#include <xc.h>

/*******************************************************************************
 * @brief Set the DACs to the initial safe-startup state
 * 
 * The offset value will be mid-scale (0 V) and waveform output 0.
 ******************************************************************************/
void InitializeWaveformDACs(void);

/*******************************************************************************
 * @brief Change the X and Y DC offset values synchronously
 * 
 ******************************************************************************/
void SetOffsets(uint16_t xVal, uint16_t yVal);

/*******************************************************************************
 * @brief Change the X and Y amplitudes synchronously
 ******************************************************************************/
void SetAmplitudes(uint16_t xVal, uint16_t yVal);

/*******************************************************************************
 * @brief Update the X offset and amplitude synchronously
 ******************************************************************************/
void SetXOffsetAndAmplitude(uint16_t xOff, uint16_t xAmp);

/*******************************************************************************
 * @brief Update just the X amplitude
 ******************************************************************************/
void SetXAmplitude(uint16_t xAmp);

/*******************************************************************************
 * @brief Update just the X offset
 ******************************************************************************/
void SetXOffset(uint16_t xOff);

/*******************************************************************************
 * @brief Update the Y offset and amplitude synchronously
 ******************************************************************************/
void SetYOffsetAndAmplitude(uint16_t yOff, uint16_t yAmp);






#endif	/* WAVEFORM_DAC_H */

