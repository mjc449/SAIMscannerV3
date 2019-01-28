/*
Library for using AD5440 dual channel 10 bit DAC on the SAIMScannerV3 hardware
The outputs are configured for bipolar, 4-quadrant multiplication with +10V 
references.  The references provide trimmers on jumpers for < 10V values.

Copyright 20160619 Marshall Colville (mjc449@cornell.edu)
*/

//Macros
#define AUX_LOAD bit_clear(LATG, 12); bit_set(LATG, 12);

//Global variables
static int AUXMidscale[2] = {0x01FF, 0x01FF};  //Midscale output for Aux DACs

//Writes the referenced value to the DAC A output
void write_aux_a(int& value)
{
   bit_clear(LATG, 13);
   if(value > 0x3FFF)  //Check that the value is within the bounds
      output_e(0x3FFF);  //If not, output max code
   else
      output_e(value);
   AUX_LOAD;
}

//Writes the referenced value to the DAC B output
void write_aux_b(int& value)
{
   bit_set(LATG, 13);
   if(value > 0x3FFF)
      output_e(0x3FFF);
   else
      output_e(value);
   AUX_LOAD;
}
