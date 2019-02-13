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

Copyright 20160619 Marshall Colville (mjc449@cornell.edu)
*/

#include "firmware_0_0.h"
#include "AD5583_dual_drvr.h"


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
