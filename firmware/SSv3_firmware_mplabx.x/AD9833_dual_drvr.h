/* 
 * File:   AD9833_dual_drvr.h
 * Author: Marshall
 *
 * Created on August 26, 2018, 6:00 PM
 */

#ifndef AD9833_DUAL_DRVR_H
#define	AD9833_DUAL_DRVR_H

//Global variables
static int OnekHz = 0x29F1;  //Default scan frequency
static int Phase90 = 0x0400;  //Default phase between x and y DDS outputs
static int Frequency = OnekHz;
static int Phase = Phase90;

void wave_freq_range(int1);
void wave_set_freq(int);
void wave_set_phase_diff(int);
void wave_reset(int1);

#endif	/* AD9833_DUAL_DRVR_H */

