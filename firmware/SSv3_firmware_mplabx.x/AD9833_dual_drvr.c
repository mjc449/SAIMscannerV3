/*
Library for using two AD9833 DDS chips
The DDSs share SYNC and SCK lines, but have separate SDI line
Serial data is shifted into both DDS units simultaneously

For the SAIMScannerV3 hardware we only ever want a sine output at < 1.1 kHz,
so control options are limited to these conditions and reset

Copyright 20160619 Marshall Colville (mjc449@cornell.edu)
*/

#include "firmware_0_0.h"
#include "AD9833_dual_drvr.h"

#bit WAVE_XDO=LATC.4
#bit WAVE_YDO=LATG.8

//Global Variables
static int ControlWord = 0;

void wave_xfer(int& XMessage, int& YMessage)  //Function to transfer data
{
   bit_clear(LATG, 7); //Active low serial frame sync, stays low for 16 clocks
   for (int i = 0; i < 16; i++)
   {
      bit_set(LATG, 6);
      //Write the leftmost bit to the appropriate pin and shift in a 0 from right
      WAVE_XDO = shift_left(&XMessage, 2, 0);
      WAVE_YDO = shift_left(&YMessage, 2, 0);
      //Pulse the clock
      bit_clear(LATG, 6);
   }
   bit_set(LATG, 7);  //End of transfer, raise sync
   bit_set(LATG, 6);  //Idle the SCK
}

//Function to set the reset bit
void wave_reset(int1 RSState)
{
   if(RSState)  //State = 1 enables reset, 0 clears reset
      bit_set(ControlWord, 8);
   else
      bit_clear(ControlWord, 8);
   int XMessage = ControlWord;
   int YMessage = ControlWord;
   wave_xfer(XMessage, YMessage);
}

//Function to set the MSB or LSB of freq registers
//For the SAIM scanner with 25 MHz MCLK writing 0x00 to the MSB limits the
//maximum output freq to 1525 Hz, which is faster than we can run the galvs.
//Therefore, we write 0x00 once to the MSB at startup, then all writes
//during operation are to the LSB
void wave_freq_range(int1 HighLow)
{
   if(HighLow)  //HighLow=1 writes to 14 MSB of freq reg, 0 writes to LSB
      bit_set(ControlWord, 12);
   else
      bit_clear(ControlWord, 12);
   int XMessage = ControlWord;
   int YMessage = ControlWord;
   wave_xfer(XMessage, YMessage);
}

//Function to write to the frequency registers
//We only use FREQ0, and both DDS units share MCLK, so the same frequency is
//written to both chips
void wave_set_freq(int value)
{
   if(value > 0x2E23)  //This corresponds to an output freq of 1.1 kHz
      value = 0x2E23;  //To avoid damaging the galvs, limit the maximum freq
   Frequency = value;
   value |= 0x4000;
   int XMessage = value;
   int yMessage = value;
   wave_xfer(XMessage, YMessage);
}

//Function to write the phase offset between the DDS chips
//Because we are scanning a circle the phase offset in Y begins at pi/2
//Therefore, the y phase will be value + pi/2
void wave_set_phase_diff(int value)
{
   if(value > 0x0FFF)  //Maximum allowable phase
      value = 0x0FFF;  //Limit to maximum
   int XMessage = 0xC000;  //Phase X = 0;
   int YMessage = value | 0xC000;  //Phase Y = value + pi/2
   wave_xfer(XMessage, YMessage);
   Phase = value;
}
