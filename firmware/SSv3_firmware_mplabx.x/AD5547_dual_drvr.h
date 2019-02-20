/* 
 * File:   AD5547_dual_drvr.h
 * Author: Marshall
 *
 * Created on August 26, 2018, 5:48 PM
 */

#ifndef AD5547_DUAL_DRVR_H
#define	AD5547_DUAL_DRVR_H

#include <stddef.h>
#include "firmware_0_0.h"

//Quick macros to handle basic operations
#define X_AMP bit_set(LATA, 9); bit_set(LATA, 10);
#define X_DC bit_clear(LATA, 9); bit_clear(LATA, 10);
#define Y_AMP bit_clear(LATA, 9); bit_clear(LATA, 10);
#define Y_DC bit_set(LATA, 9); bit_set(LATA, 10);
#define X_WRITE bit_clear(LATF, 13); bit_set(LATF, 13);
#define Y_WRITE bit_clear(LATF, 4); bit_set(LATF, 4);
#define GDAC_LOAD bit_set(LATF, 12); bit_clear(LATF, 12);
#define GDAC_RESET bit_clear(LATA, 1); bit_set(LATA, 1);

//Global variables
static int CSCenter[] = {0x7FFF, 0x7FFF};  //Storage for the circle-scan center
static int CSRadius[] = {0x0000, 0x0000};  //Storage for the circle-scan radius
static int CSTIRF[] = {0x3D00, 0x3D00};  //Initialized to approximate TIRF for 100x NA 1.49, 300 mm TL, 10x BE
static int DiscSineX [32];  //Storage for a discretized sine wave
static int DiscSineY [32];
static int Tmr2Reset = 0x0000;  //Reset value on TMR2 overflow for point scanning
static int* PointListX = NULL;  //Pointers to the lists of points to scan
static int* PointListY = NULL;
static int ScanPosition = 0;  //Current value in the point scanning loop
static int ScanRollover = 0;  //Rollover value for the point scanning loop

static int32 UnitCircle[] = 
   {
   0xFFFF, 0xFD89, 0xF640, 0xEA6C, 0xDA81, 0xC71C, 0xB0FB, 0x98F8,
   0x7FFF, 0x6706, 0x4F03, 0x38E2, 0x257D, 0x1592, 0x09BE, 0x0275,
   0x0000, 0x0275, 0x09BE, 0x1592, 0x257D, 0x38E2, 0x4F03, 0x6706,
   0x7FFF, 0x98F8, 0xB0FB, 0xC71C, 0xDA81, 0xEA6C, 0xF640, 0xFD89
   };

//Declarations
void center_park(void);
void x_offset(int*);
void y_offset(int*);
void y_amplitude(int*);
void x_amplitude(int*);
void park_center(void);
void set_scan_center(int*);
void set_scan_radius(int*);
void load_CS_radius(int8*);
void load_CS_center(int8*);
void load_CS_TIRF(int8*);
void circle_scan(void);
void TIRF_scan(void);
void park_location(int8*);
void discrete_circle_scan(int8*);
void stop_discrete_scan(void);


#endif	/* AD5547_DUAL_DRVR_H */

