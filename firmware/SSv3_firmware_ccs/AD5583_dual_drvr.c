/*
Library for using two AD5583 quad channel 10 bit DACs for DC output 
to the AOTF controller on the SAIMScannerV3 hardware.
The chips are configured for simultaneous synchronous loads, data must be
latched serially

Basic IO macros are defined in firmwave_0_0.h:
   X_AMP sets the X amplitude address bits
   X_DC sets the X DC bias address bits
   Y_AMP sets the Y amplitude address bits
   Y_DC sets the Y DC bias address bits
   
   X_WRITE pulses the X write pin
   Y_WRITE pulses the Y write pin
   GDAC_LOAD pulses the load pin on both DACs, updating all 4 outputs

Copyright 2019 Marshall J. Colville (mjc449@cornell.edu)

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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

//Update the data registers on a ADAC channel, must be followed an external call to ADAC_LOAD
void update_ADAC_channel(int8* pChannel, int* pValue)
{

   if(*pValue > 0x03FF)  //Check that the value is within range
      *pValue = 0x03FF;  //If it's too big, set it to max
   
   switch(*pChannel)
   {
      case 7:
         output_d(0b1111000000000000 | *pValue);
         ADAC1_WR;
         break;
      case 6:
         output_d(0b1111010000000000 | *pValue);
         ADAC1_WR;
         break;
      case 5:
         output_d(0b1111100000000000 | *pValue);
         ADAC1_WR;
         break;
      case 4:
         output_d(0b1111110000000000 | *pValue);
         ADAC1_WR;
         break;
      case 3:
         output_d(0b1111000000000000 | *pValue);
         ADAC0_WR;
         break;
      case 2:
         output_d(0b1111010000000000 | *pValue);
         ADAC0_WR;
         break;
      case 1:
         output_d(0b1111100000000000 | *pValue);
         ADAC0_WR;
         break;
      case 0:
         output_d(0b1111110000000000 | *pValue);
         ADAC0_WR;
         break;
      default:
         output_error(5);
         break;
   }
}

//Update all 8 AOTF channels, must be followed by an external call to ADAC_LOAD
void update_ADAC_all(int* pProfile)
{
   if(pProfile)  //Check that the profile exists
   {
      int8 channel = 0;
      for(int i = 0; i <= 7; i++)
      {
         update_ADAC_channel(&channel, pProfile++);
         channel++;
      }
   }
   else  //If the profile is NULL (deleted or unassigned)
      output_error(5);  //AOTF errors are 5 blinks
}

//Creates a new ADAC profile on the heap and returns a pointer to the profile
//Returns 0 if the operation succeeds, 1 if it fails
int new_ADAC_profile(int8* pCommand)
{
   int num = *pCommand++;
   for (int i = 0; i <= 7; i++)  //Add the value for each channel
      AOTFArray[num][i] = make16(*(pCommand++),*(pCommand++));
   return(0);
}

int load_AOTF_profile(int Profile)
{
   if(Profile >= MaxAOTF)
   {
      output_error(5);
      return 2;
   }
   update_ADAC_all(&AOTFArray[Profile][0]);  //Load the requested profile
   ADAC_LOAD;  //Update the DAC output
   return 0;
}
