/*
Firmware for the SAIMscannerV3.0 embedded microscope controller.
Copyright 20160726 by Marshall Colville (mjc449@cornell.edu)
*/


#include "firmware_0_0.h"

//Peripheral drivers
#include "AD5440_drvr.h"
#include "AD5547_dual_drvr.h"
#include "AD5583_dual_drvr.h"
#include "AD9833_dual_drvr.h"
#include "SAIM_utilities.h"
#include "Simple_SAIM.h"


#int_EXT1 LEVEL=5
//ISR that operates the AOTF global blank (gated illumination)
//and increments the AOTF profile and scan angle in a SAIM experiment
void  camera_fire_isr(void)
{
   static SAIMnode* ThisNode;
   static int* ThisStep;
   //DO_1 = 1;  //For debugging
   disable_interrupts(int_EXT1);  //Necessary to check pin state and change edge
   if((FIRE_IN || (Flags.SWTrigger && Flags.SWTriggerState))&& Flags.Ts)  //Fire signal is asserted and the galvos have settled
   {
      AOTF_SHT = 1;  //Raise the AOTF global clear
      
      //We always have a ms or more between the start of exposure and end, so this doesn't need to be super fast
      DO_0 = MirrorDetectorTrigger;  //Fire the trigger signal to the mirror detector camera
      ext_int_edge(1, H_TO_L);  //Switch to falling edge detection
   }
   else if(!FIRE_IN || (Flags.SWTrigger && !Flags.SWTriggerState))  //Fire signal has gone low
   {
      if(!Flags.AlwaysOpen) AOTF_SHT = 0;  //Lower the AOTF global clear
      ext_int_edge(1, L_TO_H);  //Switch to rising edge detection
      if(Flags.SAIM)  //SAIM experiment has been started
      {
         if(StartNode)  //If this is the first exposure in the experiment the start node sill exists
         {
            ThisNode = StartNode;
            StartNode = NULL;
            ThisStep = StartStep;
            StartStep = NULL;
         }
         
         set_scan_radius(ThisStep);  //Update the scan radius first, can do everything else while settling
         GDAC_LOAD;  //Load the new values into the DAC registers
         WAIT_TS(); //Start the settling timer
         DO_0 = 0;  //Reset the mirror detector trigger
         //Check wheter or not to fire the mirror detector on the next exposure
         MirrorDetectorTrigger =
            ((*ThisStep == *MirrorDetectorRadius) 
            && Flags.UseMirrorDetector) ? 1 : 0;
         if(Flags.LastFrame && !Flags.EndOfExp)
         {
            update_ADAC_all(ThisNode->pAOTF);  //Change the AOTF output (Ts >> update)
            ADAC_LOAD;
            Flags.LastFrame = 0;
         }
         else if(Flags.LastFrame && Flags.EndOfExp)
         {
            Flags.SAIM = Flags.EndOfExp = Flags.LastFrame = LED_EXP = 0;
            return;
         }
         
         if(ThisStep >= ThisNode->pSeqEnd)  //When we get to the end of the AOI sequence...
         {
            Flags.LastFrame = 1;  //Set the last frame flag
            if(Flags.SAIMLoop && ThisNode->pLoop != NULL)  //If this is a looping experiment and the loop node is defined
            {
               ThisNode = ThisNode->pLoop;  //Update the current node to the loop node
               ThisStep = ThisNode->pSeqStart;  //Move the step pointer to the beginning of the new sequence
            }
            else if(ThisNode->pNext != NULL)
            {
               ThisNode = ThisNode->pNext;  //Go to the next node
               ThisStep = ThisNode->pSeqStart;  //Move the step pointer to the beginning of the new sequence
            }
            else
               Flags.EndOfExp = 1;
         }
         else
            ThisStep += 2;  //This is not the end of the sequence, so we increment the AOI pointer by 2
      }
      if(Flags.SimpleSAIM)  //If a simple (non-circle) SAIM experiment
      {
         if(currStep < steps) currStep++;
         if(currStep == steps) 
         {
            Flags.SimpleSAIM = LED_EXP = 0;
            return;
         }
         if(direction!=0) x_offset(stepListX + currStep);  //Direction = 1, scan x, pointer arithmetic
         else y_offset(stepListY + currStep);  //Direction = 0, scan y, pointer arithmetic
         GDAC_LOAD;
         WAIT_TS();
      }
   }
   //In the event that the next exposure starts prior to the galvos settling
   //it will be ignored and an error output.  This is most important for high-speed
   //SAIM experiments where Ts approaches 1% of the exposure time, but up to 10%
   //of the fire signal.  Waiting for the settling time will alter the effctive
   //length of illumination, leading to artifacts in some frames, therefore the
   //frames without settled galvos will not trigger the AOTF global blank,
   //and should be discarded.  The following exposure will trigger the next step in the experiment
   else
   {
      output_error(1);  //1 blink for the dropped frame
      ext_int_edge(1, L_TO_H);  //The acquisition is aborted, so reset the edge detection
   }
   enable_interrupts(int_EXT1);  //Re-enable the interrupt
   //DO_1 = 0;  //For debugging
}

#int_EXT2 LEVEL=4
void  camera_arm_isr(void) 
{
   if(!Flags.Ts)  //Ts hasn't passed since the last move
      set_timer4(get_timer1() - ArmDelay);  //Delay before next camera trigger
   else
      set_timer4(0xFFFF - ArmDelay);  //Use the minimum delay
   enable_interrupts(int_TIMER4);  //Start the delay timer
   disable_interrupts(int_EXT2);  //This must be reset externally
}

//This holds the delay for the galvo settling time (~120 us)
//While this timer is active an exposure cannot be triggered
#int_TIMER1 LEVEL=5
void galvo_settling_isr(void)
{
   Flags.Ts = 1;  //Set the Ts bit
   disable_interrupts(int_TIMER1);  //This interrupt must be reset externally
}

//This interrupt controls the point-scanning behavior of the galvos
#int_TIMER2 LEVEL=5
void point_scan_isr(void)
{
   set_timer2(Tmr2Reset);  //Update rate = 0.25 us * (0xFFFF - Tmr2Reset)
   x_offset(PointListX + ScanPosition);  //Load the X value into the XDAC data register
   y_offset(PointListY + ScanPosition);  //Load the Y value into the YDAC data register
   GDAC_LOAD;  //Load the DAC registers simultaneously
   ScanPosition++;  //Increment the circular position
   if(ScanPosition > ScanRollover)  //When the circle is complete
      ScanPosition = 0;  //Start back at 0
}

//This interrupt controls the ERR message output on the PWR LED
#int_TIMER3 LEVEL = 1
void error_output_isr(void)
{
   LED_PWR = ~LED_PWR;  //Toggle the PWR LED on each call
   ErrBlink++;  //Increment the blink counter
   if(ErrBlink > ErrMSG)  //If the appropriate number of blinks has been reached
   {
      disable_interrupts(int_TIMER3);  //Clear the interrupt enable bit
      ErrBlink = 0;  //Reset the blink counter
      LED_PWR = 1;  //Make sure to leave the light on
   }
}

//Delay between arm signal and beginning the next exposure
//This will begin an exposure or sequence when using the microscope camera
//in external trigger mode depending on the camera configuration
#int_TIMER4 LEVEL=3  //This is lower priority than the fire ISR
void arm_delay_isr(void)
{
   DO_0 = 1;  //Trigger the start of the acquisition sequence
   disable_interrupts(int_TIMER4);
}

#int_TIMER5 LEVEL = 1  //This is the lowest priority
void general_timer_int(void)
{
   if(Flags.Paused)
      LED_EXP = ~LED_EXP;
   if(Flags.SWTrigger){
      Flags.SWTriggerState = 0;
      camera_fire_isr();
      Flags.SWTrigger = 0;
   }
}

void main()
{
   initialization () ;
   
   while (TRUE)
   {  
   if(usb_kbhit(1))
      process_command();
   restart_wdt();
   }
}

//Called from main() on an incoming USB packet
//The command and data are first fetched into the command buffer, then
//the appropriate action is taken
void process_command()
{
   int ExpNum;
   int8 result;  //Pass/Fail results
   int8 command[64]; //Buffer for incoming data
   usb_gets(1, command, 64, 100);
   switch (command[0]) //First byte is the command ID
   {
    //0x0X control commands
      case CMD_BLINK_PWR:  //Simple 10 fast blinks of the PWR/ERR LED
         for(int i = 0; i < 10; i++)
         {
            LED_PWR = 0;
            delay_ms(100);
            LED_PWR = 1;
            delay_ms(100);
         }
         break;
      case CMD_VISOR:  //Run the visor() LED function
         visor();
         break;
      case CMD_BLINK_PWR_VAR:  //Use the ERR function
         output_error(make16(command[1], command[2]));
         break;
   //0x1X scan commands
      case CMD_SET_RADIUS:
         load_CS_radius(&command[1]);
         break;
      case CMD_SET_CTR:
         load_CS_center(&command[1]);
         break;
      case CMD_SET_TIRF:
         load_CS_TIRF(&command[1]);
         break;
      case CMD_CIRCLE_SCAN:
         circle_scan();
         break;
      case CMD_TIRF_SCAN:
         TIRF_scan();
         break;
      case CMD_LOC_PARK:
         park_location(&command[1]);
         break;
      case CMD_DISC_SCAN:
         discrete_circle_scan(&command[1]);
         break;
      case CMD_DISC_SCAN_OFF:
         stop_discrete_scan();
         break;
      case CMD_CENTER_PARK:
         park_center();
         break;
   //0x2X DDS commands
      case CMD_SET_FREQ:
         wave_set_freq(make16(command[1], command[2]));
         break;
      case CMD_DEFAULT_FREQ:
         wave_set_freq(OnekHz);
         break;
      case CMD_SET_PHASE:
         wave_set_phase_diff(make16(command[1], command[2]));
         break;
      case CMD_DEFAULT_PHASE:
         wave_set_phase_diff(Phase90);
         break;
      case CMD_MCLK_TOGGLE:
         WAVE_MCLK = ~WAVE_MCLK;
         break;
      case CMD_WAVE_RS:
         wave_reset(1);
         break;
      case CMD_WAVE_CLR_RS:
         wave_reset(0);
         break;
   //0x3X auxiliary DAC
      case CMD_CONST_AUX:
         int AUXValue = make16(command[2], command[3]);
         if(!command[1])
            write_aux_a(AUXValue);
         else
            write_aux_b(AUXValue);
         break;
      case CMD_MID_AUX:
         write_aux_a(AUXMidscale[0]);
         write_aux_b(AUXMidscale[1]);
         break;
      case CMD_ZERO_AUX:
         write_aux_a(Zero[0]);
         write_aux_b(Zero[1]);
         break;
   //0x4X AOTF
      case CMD_GLOBAL_HIGH:
         FIRE_OFF();
         AOTF_SHT = 1;
         OPEN_SHUTTER;
         break;
      case CMD_GLOBAL_LOW:
         FIRE_OFF();
         AOTF_SHT = 0;
         break;
      case CMD_CHANGE_CH:
         int ChValue = make16(command[2], command[3]);
         update_ADAC_channel(&(command[1]), &ChValue);
         ADAC_LOAD;
         break;
      case CMD_LOAD_PROFILE:
         command[2] = load_AOTF_profile(command[1]);
         break;
      case CMD_OPEN_SHUTTER:
         OPEN_SHUTTER;
         break;
      case CMD_CLOSE_SHUTTER:
         FIRE_OFF();
         CLOSE_SHUTTER;
         break;
      case CMD_TOGGLE_OPEN:
         if(Flags.AlwaysOpen)
         {
            AOTF_SHT = 0;
            CLOSE_SHUTTER;
            Flags.AlwaysOpen = 0;
         }
         else
         {
            AOTF_SHT = 1;
            OPEN_SHUTTER;
            Flags.AlwaysOpen = 1;
         }
         break;
      case CMD_ADD_PROFILE:
         result = new_ADAC_profile(&command[1]);
         command[0] = make8(result, 0);
         break;
      case CMD_AOTF_RESET:
         ADAC_RS;
         AOTF_SHT = 0;
         break;
   //0x5X interrupt
      case CMD_FIRE_ON:
         FIRE_ON();
         break;
      case CMD_FIRE_OFF:
         FIRE_OFF();
         break;
      case CMD_SW_TRIGGER:
         WAIT_SWTRIGGER(make16(command[1], command[2]));
         break;
   //0x6X triggering
   
   //0x7X SC<->SC communications
   
   //0x8X experiment
      case CMD_GET_SEQ_USB:
         result = get_seq_usb(&command[1]);
         command[0] = make8(result, 0);
         break;
      case CMD_DEL_SEQ:
         int SeqNum = (int)command[1];
         delete_seq(SeqNum);
         break;
      case CMD_ADD_SEQ_LIN:
         result = add_seq_linear(&command[1]);
         command[0] = make8(result, 0);
         break;
      case CMD_ADD_EXP:
         result = create_new_node(&command[1]);
         command[0] = result;
         break;
      case CMD_ADD_NODE_START:
         result = push_node(&command[1]);
         command[0] = result;
         break;
      case CMD_ADD_NODE_END:
         result = add_last_node(&command[1]);
         command[0] = result;
         break;
      case CMD_ADD_LOOP:
         ExpNum = (int)command[1];
         int LoopNode = (int)command[2];
         result = build_loop(ExpNum, LoopNode);
         command[0] = result;
         break;
      case CMD_START_EXP:
         command[0] = start_experiment(&command[1]);
         break;
      case CMD_PAUSE_EXP:
         pause_experiment();
         break;
      case CMD_RESUME_EXP:
         resume_experiment();
         break;
      case CMD_RESTART_EXP:
         restart_experiment();
         break;
      case CMD_DEL_EXP:
         ExpNum = (int)command[1];
         delete_all_nodes(ExpNum);
         break;
      case CMD_DEL_NODE_START:
         ExpNum = (int)command[1];
         pop_node(ExpNum);
         break;
      case CMD_DEL_NODE_END:
         ExpNum = (int)command[1];
         remove_last_node(ExpNum);
         break;
      case CMD_COUNT_STEPS:
         count_steps(&command[1]);
         break;
      case CMD_STOP_EXP:
         stop_experiment();
         break;
         
   //0x9X SimpleSAIM
      case CMD_LOAD_SIMPLE_HALF:
         steps = make16(command[1], command[2]);
         int stepSize = make16(command[3], command[4]);
         command[0] = create_simple_half(stepSize);
         break;
      case CMD_LOAD_SIMPLE_FULL:
         steps = make16(command[1], command[2]);
         int stepSize1 = make16(command[3], command[4]);
         int startPosX = make16(command[5], command[6]);
         int startPosY = make16(command[7], command[8]);
         command[0] = create_simple_full(stepSize1, startPosX, startPosY);
         break;
      case CMD_DIRECTION:
         direction = command[1];
         break;
      case CMD_START_SIMPLE:
         start_simple();
         break;
      case CMD_START_DITHERED:
         int val = make16(command[1], command[2]);
         start_dithered(&val);
         break;
      case CMD_STOP_SIMPLE:
         stop_simple();
         break;
      case CMD_STEP_COUNT:
         int8* ptr = &steps;
         command[1] = *ptr++;
         command[2] = *ptr;
         ptr = &currStep;
         command[3] = *ptr++;
         command[4] = *ptr;
         break;
   
   //0xAX Mirror Detector
      case CMD_SET_MD_RADIUS:
         *MirrorDetectorRadius = make16(command[1], command[2]);
         break;
      case CMD_MD_ON:
         Flags.UseMirrorDetector = 1;
         command[1] = Flags.UseMirrorDetector;
         break;
      case CMD_MD_OFF:
         Flags.UseMirrorDetector = 0;
         command[1] = Flags.UseMirrorDetector;
         break;
         
   //0xFX special functions
      case CMD_TS_PERIOD:
         TsReset = make16(command[1], command[2]);
         break;
      case CMD_GET_SETTINGS:
         report_settings(&command[1]);
         break;
      case CMD_GET_INFO:
         command[1] = BRDVER_MAJOR;
         command[2] = BRDVER_MINOR;
         command[3] = FWVER_MAJOR;
         command[4] = FWVER_MINOR;
         break;
      case CMD_CHECK_MEM:
         check_memory_exists(&command[1]);
         break;
      case CMD_SEND_STAT:
         command[1] = Flags;
         break;
      case CMD_RESET_CPU:
         reset_cpu();
         break;
         
      default:  //If the command is not recognized (i.e. doesn't exist)
         output_error(10);  //Blink the ERR LED 10 times
         command[0] = 0xFF;  //Report the error
         break;
   }
   usb_puts (1, command, 64, 100);
}

#inline void FIRE_ON(void)
{
   if(!Flags.Fire)
   {
      OPEN_SHUTTER;
      AOTF_SHT = 0;
      Flags.Fire = 1;
      enable_interrupts(int_EXT1);
   }
}

#inline void FIRE_OFF(void)
{
   if(Flags.Fire)
   {
      pause_experiment();
      disable_interrupts(int_EXT1);
      ext_int_edge(1, L_TO_H);
      AOTF_SHT = 0;
      Flags.Fire = 0;
   }
}

#inline void WAIT_TS(void)
{
   Flags.Ts = 0;
   set_timer1(TsReset);
   enable_interrupts(int_TIMER1);
}

#inline void WAIT_SWTRIGGER(int val)
{
   Flags.SWTrigger = 1;
   Flags.SWTriggerState = 1;
   camera_fire_isr();
   set_timer5(val);
   enable_interrupts(int_TIMER5);
}

void report_settings(int8 *pCommand)
{
   *pCommand++ = make8(CSCenter[0], 1);
   *pCommand++ = make8(CSCenter[0], 0);
   *pCommand++ = make8(CSCenter[1], 1);
   *pCommand++ = make8(CSCenter[1], 0);
   *pCommand++ = make8(CSTIRF[0], 1);
   *pCommand++ = make8(CSTIRF[0], 0);
   *pCommand++ = make8(Phase, 1);
   *pCommand++ = make8(Phase, 0);
   *pCommand++ = make8(Frequency, 1);
   *pCommand++ = make8(Frequency, 0);
}

//Asks whether the pointers in the head arrays are assigned
//Returns the number of assigned pointers, 0 otherwise
//Also overwrites the command buffer to show number of assignments
void check_memory_exists(int8* pCommand)
{
   int8 ExpInUse = 0;
   long ExpPattern = 0;
   int i;
   for(i = 0; i < MaxExp; i++)
   {
      if(ExpHeads[i])
      {
         ExpInUse++;
         bit_set(ExpPattern, i);
      }
   }
   *pCommand++ = ExpInUse;
   *pCommand++ = make8(ExpPattern, 3);
   *pCommand++ = make8(ExpPattern, 2);
   *pCommand++ = make8(ExpPattern, 1);
   *pCommand++ = make8(ExpPattern, 0);
}

void visor()
{
   int1 PWRstate = LED_PWR;
   int1 SHTstate = LED_SHT;
   int1 SCNstate = LED_SCN;
   int1 EXPstate = LED_EXP;
   for(int i = 0; i < 5; i++)  //Visor output for panel LEDs
   {
      LED_PWR = 1;
      delay_ms(50);
      LED_PWR = 0;
      LED_SHT = 1;
      delay_ms(50);
      LED_SHT = 0;
      LED_SCN = 1;
      delay_ms(50);
      LED_SCN = 0;
      LED_EXP = 1;
      delay_ms(50);
      LED_EXP = 0;
      LED_SCN = 1;
      delay_ms(50);
      LED_SCN = 0;
      LED_SHT = 1;
      delay_ms(50);
      LED_SHT = 0;
      LED_PWR = 1;
      delay_ms(50);
      LED_PWR = 0;
   }
   LED_PWR = PWRstate;
   LED_SHT = SHTstate;
   LED_SCN = SCNstate;
   LED_EXP = EXPstate;
}

//Output a specified number of blinks on the PWR/ERR LED
void output_error(int Blinks)
{
   ErrMSG = (2 * Blinks) - 1;
   set_timer3(0x0000);
   enable_interrupts(int_TIMER3);
}

//Function to setup the I/O ports and initialize the peripherals
void initialization()
/*
This function is called once at powerup to initialize peripherals and put
the system in a known state.  All unused pins are set as outputs and driven 
*/
{
   //Using FAST_IO directive requires manual TRIS config
   //Setup PORTA
   bit_set(TRISA,15);  //TRIG_1
   bit_set(TRISA,14);  //TRIG_0
   bit_clear(TRISA,10);  //GDAC A1
   bit_clear(TRISA,9);  //GDAC A0
   bit_clear(TRISA,7);  //AOTF global blank
   bit_clear(TRISA,6);  //Unused
   bit_clear(TRISA,5);  //LED_EXP
   bit_clear(TRISA,4);  //LED_SCN
   bit_clear(TRISA,3);  //LED_SHT
   bit_clear(TRISA,2);  //LED_PWR
   bit_clear(TRISA,1);  //GDAC_RS
   bit_clear(TRISA,0);  //Unused
   set_pulldown(true, PIN_A15);  //A15 and A14 have external pulldowns
   set_pulldown(true, PIN_A14);  //Internal pulldowns might interfere withinputs
   LATA = 0b0000000000000100; //Drive everything low except the PWR/STATUS LED
   bit_set(LATA, 1);      //Bring the GDAC out of reset
   
   //Setup PORTB
   SET_TRIS_B(0); //PORTB is the 16 bit GDAC data port, nothing to do here
   output_b(0x7FFF);  //The GDAC is already at midrange reset, but might as well...
   
   //Setup PORTC
   bit_clear(TRISC,14);  //C14 and C13 are GPIO pins connected to panel BNCs
   bit_clear(TRISC,13);  //We use these as trigger sources for other hardware
   bit_clear(TRISC,4);  //C4 is the X DDS data line
   bit_clear(TRISC,3);  //Unused
   bit_clear(TRISC,2);  //Unused
   bit_clear(TRISC,1);  //Unused
   output_c(0b0000000000010000);  //Drive everything low except the X DDS data line
   
   //Setup PORTD
   SET_TRIS_D(0);  //PORTD is the 10 bit ADAC data port and control pins
   output_d(0b0111000000000000);  //Bring, LOAD, and CS lines high, everything else low
   bit_set(LATD, 15);          //Bring the RS high to enable output
   
   //Setup PORTE
   SET_TRIS_E(0);  //PORTE is the 10 bit AUXDAC data bus, nothing to do here
   output_e(0x01FF);  //Set data pins to midrange
   
   //Setup PORTF
   bit_clear(TRISF,13);  //PORTF has PIC2PIC SPI as well as VBUS, so we use STD
   bit_clear(TRISF,12);  //IO on it, however we will set tris and toggle LATF
   bit_clear(TRISF,4);   //for the PINS that we are manually controlling
   bit_clear(TRISF,1);
   bit_clear(TRISF,0);
   bit_set(LATF, 13);  //GDAC_XWR is active low (latches data on falling edge)
   bit_clear(LATF, 12);  //GDAC_LD is active high (loads data on rising edge)
   bit_set(LATF, 4);  //GDAC_YWR
   bit_clear(LATF, 1);  //Unused
   bit_clear(LATF, 0);  //Unused
   
   //Setup PORTG
   bit_clear(TRISG,15);  //PORTF has the D+/D- pins, so we have to use STD IO
   bit_clear(TRISG,14);  //We still set TRIS in the initialization sequence
   bit_clear(TRISG,13);  //on the data pins that we will manually control
   bit_clear(TRISG,12);  //for peripherals
   bit_clear(TRISG,9);
   bit_clear(TRISG,8);
   bit_clear(TRISG,7);
   bit_clear(TRISG,6);
   bit_clear(TRISG,1);
   bit_clear(TRISG,0);
   bit_clear(LATG, 15);  //Set the startup states
   bit_clear(LATG, 14);  //Unused pins should be left low
   bit_clear(LATG, 13);  //Select channel A
   bit_set(LATG, 12);  //Diasble input register
   bit_set(LATG, 9);  //Begin with 25 MHz oscillator output enabled
   bit_set(LATG, 8);;  //Y DDS data line idle high
   bit_set(LATG, 7);  //Active low DDS SYNC
   bit_set(LATG, 6);  //DDS SCK, data is clocked on falling edge, idle high
   bit_clear(LATG, 1);  //Unused
   bit_clear(LATG, 0);  //Unused
   //Setup the galvo DACs to center park the galvos
   center_park();
   //Set the AUXDACs to midscale (0V)
   write_aux_a(AUXMidscale[0]);
   write_aux_b(AUXMidscale[1]);
   
   delay_us(100);
   
   //Setup the DDS chips
   //We will initialize them to output two sine functions at 1 kHz with a pi/2
   //phase shift between them, which is the default operating state
   wave_reset(1);  //Enables the reset state of both DDS, disabling output
   wave_freq_range(1);  //while reset, select the freq MSB
   wave_set_freq(0x0000);  //Write 0 to the MSB, limiting output range to 1.5 kHz
   wave_freq_range(0);  //Select the freq LSB
   wave_set_freq(OnekHz);  //Set the output freq to 1 kHz
   wave_set_phase_diff(Phase90);  //Set the phase offset to pi/2
   wave_reset(0);  //Disables reset, output becomes available
   
   for(int i = 0; i < 5; i++)  //Blink the shutter LED to verify DDS have been programmed
   {
      LED_SHT = 1;
      delay_ms(100);
      LED_SHT = 0;
      delay_ms(100);
   }
   
   usb_init();  //Start the USB module
   setup_timer1(TMR_INTERNAL | TMR_DIV_BY_1);  //The galv settling timer
   setup_timer2(TMR_INTERNAL | TMR_DIV_BY_1);  //The galv move timer for point scanning
   setup_timer3(TMR_INTERNAL | TMR_DIV_BY_64);  //The error message output timer
   setup_timer4(TMR_INTERNAL | TMR_DIV_BY_1);  //Arm delay timer
   setup_timer5(TMR_INTERNAL | TMR_DIV_BY_8);  //General purpose timer/interrupt source
   ext_int_edge(1, L_TO_H);  //Start looking for the rising edge of the fire signal
   ext_int_edge(2, L_TO_H);  //Camera is ready when arm goes high
   //enable_interrupts(GLOBAL);

   visor();  //Indicate the initialization is finished
   LED_PWR = 1;
   
   Flags.Ts = 1;
   Flags.SAIM = 0;
   Flags.Paused = 0;
   Flags.Fire = 0;
   Flags.Arm = 0;
   Flags.DiscScan = 0;
   Flags.SWTrigger = 0;
   Flags.SWTriggerState = 0;
   
   setup_wdt(WDT_16S);
}
