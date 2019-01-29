/*
Driver for doing a simple SAIM experiment walking through
a set of angles from 0 deg to a user defined upper limit.

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

//Global variables
static int currStep;      //Current step number
static int steps;         //Number of steps in the experiment
static int8 direction;     //Direction to step in
const int maxSteps = 128;  //Maximum size of the experiment
int stepListX[maxSteps];  //List of positions to step through along X
int stepListY[maxSteps];  //List of positions to step through along Y

int create_simple(int& stepSize)
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
int create_simple(int& stepSize, int& startPosX, int&startPosY)
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

void start_simple()
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

void stop_simple()
{
   if(Flags.SimpleSAIM)
   {
      Flags.SimpleSAIM = LED_EXP = LED_SCN = 0;
      currStep = 0;
      park_center();
   }
}
