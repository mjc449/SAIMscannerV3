/*******************************************************************************
 * @file rasterscan.h
 * @brief Declarations and types for point-scanning imaging modes.
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


#ifndef RASTERSCAN_H
#define	RASTERSCAN_H

void RasterCenter(int, int);
void ScanWidth(int);
void ScanHeight(int);
void ScanlineSpacing(int);
void ScanlineSpeed(int);
void LineScanPower(int);
void SnapRaster(void);
void RunRaster(void);
void StopRaster(void);

#endif	/* RASTERSCAN_H */

