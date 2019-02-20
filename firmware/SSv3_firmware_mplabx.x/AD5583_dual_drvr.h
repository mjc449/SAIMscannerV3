/* 
 * File:   AD5583_dual_drvr.h
 * Author: Marshall
 *
 * Created on August 26, 2018, 5:59 PM
 */

#ifndef AD5583_DUAL_DRVR_H
#define	AD5583_DUAL_DRVR_H

#define ADAC_LOAD bit_clear(LATD, 14); bit_set(LATD, 14);
#define ADAC0_WR bit_clear(LATD, 12); bit_set(LATD, 12);
#define ADAC1_WR bit_clear(LATD, 13); bit_set(LATD, 13);
#define ADAC_RS bit_clear(LATD, 15); bit_set(LATD, 15);
#define CHA bit_clear(LATD, 11); bit_clear(LATD, 10);
#define CHB bit_clear(LATD, 11); bit_set(LATD, 10);
#define CHC bit_set(LATD, 11); bit_clear(LATD, 10);
#define CHD bit_set(LATD, 11); bit_set(LATD, 11);

//Global variables
const size_t MaxAOTF = 32;
static int* AOTFArray[MaxAOTF][8];  //Addresses of the AOTF profile locations on the heap

//Declarations
void update_ADAC_channel(int8*, int*);
void update_ADAC_all(int*);
int new_ADAC_profile(int8*);
int load_AOTF_profile(int);

#endif	/* AD5583_DUAL_DRVR_H */

