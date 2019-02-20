
//Driver for doing a simple SAIM experiment walking through
//a set of angles from 0 deg to a user defined upper limit.
//
//Copyright 20161213 by Marshall Colville (mjc449@cornell.edu)

#include "firmware_0_0.h"
#include "SAIM_utilities.h"
#include "AD5547_dual_drvr.h"
#include "Simple_SAIM.h"


int create_simple_half(int stepSize)
{
   int code = 0;
   if(steps > maxSteps) 
   {
      steps = maxSteps;    //Change the number of steps to the max array size
      code = 1;            //let the caller know steps were truncated
   }
   //If the steps are too big or will go past TIR
   int limit = CSCenter[0] + CSTIRF[0];
   if((stepSize > 0x0200) || (steps * stepSize > limit))
   {
      output_error(5);  //Visual error indicator
      return 2;         //Exit without building the experiment
   }
   stepListX[0] = CSCenter[0];   //If no center is specified use the
   stepListY[0] = CSCenter[1];   //current CSCenter
   for(int i = 1; i < steps; i++)
   {
      stepListX[i] = stepListX[i-1]-stepSize;
      stepListY[i] = stepListY[i-1]-stepSize;
   }
   return code;
}

//Overloaded function to specify a starting position different
//from the CSCenter position
int create_simple_full(int stepSize, int startPosX, int startPosY)
{
   int code = 0;
   int highLim = CSCenter[0] + CSTIRF[0];
   int lowLim = CSCenter[0] - CSTIRF[0];
   if(steps > maxSteps)
   {
      steps = maxSteps;
      code = 1;
   }
   if((stepSize > 0x0200) ||
      (startPosX < lowLim) ||
      (startPosY < lowLim) ||
      (startPosX + steps * stepSize > highLim) ||
      (startPosY + steps * stepSize > highLim))
   {
      output_error(5);  //Visual error indicator
      return 2;
   }
   stepListX[0] = startPosX;
   stepListY[0] = startPosY;
   for(int i = 1; i < steps; i++)
   {
      stepListX[i] = stepListX[i-1]-stepSize;
      stepListY[i] = stepListY[i-1]-stepSize;
   }
   return code;
}

void start_simple(void)
{
   center_park();
   stop_experiment();
   currStep = 0;
   Flags.SimpleSAIM = LED_EXP = 1;
   FIRE_ON();
}

void start_dithered(int* dither)
{
   center_park();
   stop_experiment();
   currStep = 0;
   Flags.SimpleSAIM = LED_EXP = LED_SCN = 1;
   if(direction) y_amplitude(dither);  //Direction = 1, scan x, dither y
   else x_amplitude(dither);  //Direction = 2, scan y, dither x
   FIRE_ON();
}

void stop_simple(void)
{
   if(Flags.SimpleSAIM)
   {
      Flags.SimpleSAIM = LED_EXP = LED_SCN = 0;
      currStep = 0;
      park_center();
   }
}
