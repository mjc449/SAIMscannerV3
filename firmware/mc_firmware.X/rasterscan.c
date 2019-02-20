/*******************************************************************************
 * @file rasterscan.c
 * @brief Definitions for point-scanning imaging modes.
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


#include "rasterscan.h"
#include <xc.h>
#include <stdbool.h>

static int scanCenter[2];
static int scanWidth;
static int scanHeight;
static int scanlineSpace;
static int scanSpeed;
static int excitationPower;
static int cycloidWaveform[4096];
static bool scanChanged;

void CalculateScanWaveform()
{
    
    scanChanged = false;
}

void RasterCenter(int xCenter, int yCenter)
{
    scanCenter[0] = xCenter;
    scanCenter[1] = yCenter;
    scanChanged = true;
}

void ScanWidth(int lineWidth)
{
    scanWidth = lineWidth;
    scanChanged = true;
}

void ScanHeight(int lineHeight)
{
    scanHeight = lineHeight;
    scanChanged = true;
}

void ScanlineSpacing(int lineSpace)
{
    scanlineSpace = lineSpace;
    scanChanged = true;
}

void ScanlineSpeed(int lineSpeed)
{
    scanSpeed = lineSpeed;
    scanChanged = true;
}


