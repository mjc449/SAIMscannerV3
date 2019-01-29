/*
Library for using two AD5547 dual channel 16 bit DACs with AD9833 DDS chips
for sine output and 10V DC refs on the SAIMScannerV3 hardware
The chips are configured for simultaneous synchronous loads, maintaining
circularity of the output scan
Functionality includes point scanning, raster scanning, and circle scanning
with elipticity control
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

//Function to set the DC offset of the x galvo
void x_offset(int* pValue)
{
   X_DC;
   output_b(*pValue);  //Write value to the parallel port
   X_WRITE;
}

//Function to set the DC offset of the y galvo
void y_offset(int* pValue)
{
   Y_DC;
   output_b(*pValue);
   Y_WRITE;
}

//Function to set the amplitude of the x sine output
void x_amplitude(int* pValue)
{
   X_AMP;
   output_b(*pValue);
   X_WRITE;
}

//Function to set the amplitude of the y sine output
void y_amplitude(int* pValue)
{
   Y_AMP;
   output_b(*pValue);
   Y_WRITE;
}

//Function to center the galvos and park them
void center_park(void)
{
   x_amplitude(&Zero[0]);
   y_amplitude(&Zero[1]);
   x_offset(&CSCenter[0]);
   y_offset(&CSCenter[1]);
   GDAC_LOAD;
}

//Function to be called from "firmware_0_0.c" to set the scan center
void set_scan_center(int* pArray)
{
   x_offset(&pArray[0]);
   y_offset(&pArray[1]);
}

//Function to be called from "firmware_0_0.c" to set the scan radius
void set_scan_radius(int* pArray)
{
   x_amplitude(&pArray[0]);
   y_amplitude(&pArray[1]);
}

//Function to calculate a discretized sine wave for comparison to DDS output
void compute_circle (int32 Radius)
   {
      int32 Offset = 0x00008000 - (Radius / 2);
      int8 YPos = 8;
      for (int i = 0; i < 32; i++)
      {
         //int32 circleValueX = unitCircle16[i];
         //int32 circleValueY = unitCircle16[y_pos];

         int XValue  =
         ( (Radius * UnitCircle[i]) / 0x0000FFFF) + Offset;

         int32 YValue  =
         ( (Radius * UnitCircle[YPos]) / 0x0000FFFF) + Offset;
         DiscSineX[i] = make16(make8(XValue, 1), make8(XValue, 0));
         DiscSineY[i] = make16(make8(YValue, 1), make8(YValue, 0));
         YPos++;
         if (YPos == 32)
         {
            YPos = 0;
         }
      }
   }
   
//Set and update the scan radius
void load_CS_radius(int8* pValues)
{
   int1 resume = 0;
   if(Flags.Fire)
      resume = 1;
   FIRE_OFF();
   CSRadius[0] = make16(*pValues++, *pValues++);
   CSRadius[1] = make16(*pValues++, *pValues);
   set_scan_radius (CSRadius);
   set_scan_center(CScenter);
   GDAC_LOAD;
   LED_SCN = 1;
   if(resume)
      FIRE_ON();
}

//Set and update the scan radius
void load_CS_center(int8* pValues)
{
   int1 resume = 0;
   if(Flags.Fire)
      resume = 1;
   FIRE_OFF();
   CSCenter[0] = make16 (*pValues++, *pValues++);
   CSCenter[1] = make16 (*pValues++, *pValues);
   set_scan_center (CSCenter);
   GDAC_LOAD;
   if(resume)
      FIRE_ON();
}

//Set and update the TIRF scan radius
void load_CS_TIRF(int8* pValues)
{
   int1 resume = 0;
   if(Flags.Fire)
      resume = 1;
   FIRE_OFF();
   CSTIRF[0] = make16(*pValues++, *pValues++);
   CSTIRF[1] = make16(*pValues++, *pValues);
   set_scan_radius(CSTIRF);
   GDAC_LOAD;
   LED_SCN = 1;
   if(resume)
      FIRE_ON();
}

//Set the scan at the stored CS values
void circle_scan(void)
{
   int1 resume = 0;
   if(Flags.Fire)
      resume = 1;
   FIRE_OFF();
   set_scan_radius (CSRadius);
   set_scan_center (CSCenter);
   GDAC_LOAD;
   LED_SCN = 1;
   if(resume)
      FIRE_ON();
}

//Set the scan at the stored TIRF values
void TIRF_scan(void)
{
   int1 resume = 0;
   if(Flags.Fire)
      resume = 1;
   FIRE_OFF();
   set_scan_radius(CSTIRF);
   set_scan_center(CSCenter);
   GDAC_LOAD;
   LED_SCN = 1;
   if(resume)
      FIRE_ON();
}

//Park the beam at a designated coordinate pair
void park_location(int8* pValues)
{
   int1 resume = 0;
   if(Flags.Fire)
      resume = 1;
   FIRE_OFF();
   int Point[2];
   Point[0] = make16(*pValues++, *pValues++);
   Point[1] = make16(*pValues++, *pValues);
   set_scan_radius(Zero);
   set_scan_center(Point);
   GDAC_LOAD;
   LED_SCN = 0;
   if(resume)
      FIRE_ON();
}

//Stop a discretized circle scan
void stop_discrete_scan(void)
{
   if(Flags.DiscScan)
   {
      disable_interrupts(int_TIMER2);
      center_park();
      delay_us(500);  //Delay to allow galvs to settle before next move
      LED_SCN = 0;
      Flags.DiscScan = 0;
   }
}

//Load and begin a discretized circle scan
void discrete_circle_scan(int8* pValues)
{
   int1 resume = 0;
   if(Flags.Fire)
      resume = 1;
   FIRE_OFF();
   stop_discrete_scan();
   Flags.DiscScan = 1;
   compute_circle(make16(*pValues++, *pValues++));
   Tmr2Reset = make16(*pValues++, *pValues);
   ScanRollover = 31;
   PointListX = &DiscSineX[0];
   PointListY = &DiscSineY[0];
   enable_interrupts(int_TIMER2);
   LED_SCN = 1;
   if(resume)
      FIRE_ON();
}
 
//Park the beam at the center position
void park_center(void)
{
   FIRE_OFF();
   center_park();
   LED_SCN = 0;
}
