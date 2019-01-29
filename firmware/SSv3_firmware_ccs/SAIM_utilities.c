/*
Function definitions and variables for loading and running SAIM experiments
on the SAIMScanner_v3.1 hardware.

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

//Define the node structure for the linked lists that make up experiments
typedef struct node {
   int* pSeqStart;  //Start of the AOI sequence
   int* pSeqEnd;  //End of the AOI sequence
   int* pAOTF;  //Head of the AOTF profile
   struct node* pNext;  //Pointer to the next node in the experiment
   struct node* pLoop;  //Pointer to the node to jump to in a looping experiment
   } SAIMnode;

//Define the experiment setup parameters structure type
//for storing and initiating experiments
typedef struct experiment {
   int ExpNum;  //Number of the experiment list in the head array
   ptrdiff_t StepNum;  //Step to begin the experiment at
   int LoopOn;  //T/F use loop if defined
   int ActivationTime;  //Time for pre-experiment photoactivation (if 0 this is skipped)
   int ActivationIntensity;  //Intensity to use for pre-experiment photoactivation
   } ExpSetup;

//Define some global variables
const size_t MaxExp = 32;  //Can be changed later if need be
static SAIMnode* ExpHeads[MaxExp];  //Locations of the experiment begin nodes
const size_t MaxSeq = 32;  //Can be changed later if need be
static int* SeqTails[MaxSeq];  //Location of the last angle (HBYTE x) in the sequences
static int SeqArray[MaxSeq][ 2 * 128];
static SAIMnode* StartNode = NULL;
static int* StartStep = NULL;
static int* MirrorDetectorRadius = NULL;
static int1 MirrorDetectorTrigger = 0;

void delete_seq(int& SeqNum)
{
   SeqTails[SeqNum] = NULL;
}

//Creates a new sequence
//Returns 0 if the operation succeeds, 1 if allocation fails, 2 if usb error
int8 get_seq_usb(int8* pCommand)
{
   int SeqNum = (int)*pCommand++;  //Second byte in the command is the list number
   int SeqLen = make16(*pCommand++, *pCommand++);  //Third and fourth bytes are the length of the sequence (number of angles)
   int PacketsInbound = make16(*pCommand++, *pCommand++);  //The number of USB packets required for the transfer
   int PacketsRead = 0;
   int8 Data[64] = {0};
   int size = 0;
   
   if(!SeqLen || (SeqLen > 128))
   {
      output_error(2);
      return(2);
   }
   /**************************************************************************/
   //The following cannot fail or undefined behavior results on StartExperiment
   delete_seq(SeqNum);  //Make sure to clean up in case the rest fails
   
   int* pSeq = &SeqArray[SeqNum][0];  //Pointer to the array that will hold the 
   
   SeqTails[SeqNum] = &SeqArray[SeqNum][(SeqLen - 1) * 2];
   /**************************************************************************/
   
   usb_puts(1, Data, 64, 100);
   int lastPacket = 0;
   do  //Loop to read in data
   {
      while(!usb_kbhit(1)){};  //Wait for the packet to be sent
      usb_gets(1, Data, 64, 100);  //Store in the data buffer
      PacketsRead++;
      if(PacketsRead < PacketsInbound)
         size = 32;  //Full report length in words
      else if(PacketsRead == PacketsInbound)
      {
         size = (SeqLen * 2) - ((PacketsRead - 1) * 32);
         lastPacket = 1;
      }
      else
      {
         output_error(2);
         return 2;
      }
      for(int i = 0; i < size; i++)
      {
         *pSeq++ = make16(Data[i * 2], Data[i * 2 + 1]);  //Put the values into the sequence
      }
      usb_puts(1, Data, 64, 100);  //Response to signal ready for next transfer
      restart_wdt();
   }while(lastPacket == 0);
   
   return(0);
}

int8 add_seq_linear(int8* pCommand)
{
   int SeqNum = make16(0, *pCommand++);  //Second byte in the command is the list number
   int SeqLen = make16(*pCommand++, *pCommand++);  //Third and fourth bytes are the number of angles in the sequence
   int SeqStep = make16(*pCommand++, *pCommand++);  //Fourth and fifth bytes are the step size for the sequence in DAC units
   int SeqStart = make16(*pCommand++, *pCommand ++);  //Sixth and seventh bytes are the first angle in the sequence
   int YScale = make16(*pCommand++, *pCommand++);  //Eighth and ninth bytes are a linear adjustment to the y values to account for ellipticity
   int* pSeq = NULL;  //Pointer to the new sequence
   int MidPt = 0x7FFF;
   int XValue = SeqStart;
   
   if(!SeqLen || (SeqLen > 128))
   {
      output_error(2);
      return(2);
   }
   
   //Use the scale factor to calculate the y start
   int YValue = (int)((float)SeqStart * (float)YScale / (float)MidPt);
   
   //Calculate the step value for the Y DAC.  This is the most basic correction and may not be adequate
   int YStep = (int)((float)SeqStep * (float)YScale / (float)MidPt);
   
   
   /**************************************************************************/
   //The following cannot fail or undefined behavior results on StartExperiment
   delete_seq(SeqNum);  //Make sure to clean up in case the rest fails
   
   pSeq = &SeqArray[SeqNum][0];  //Pointer to the array that will hold the 
   
   SeqTails[SeqNum] = &SeqArray[SeqNum][(SeqLen - 1) * 2];
   /**************************************************************************/
   
   for(int i = 0; i < SeqLen; i++)
   {
      *pSeq++ = XValue;  //Put the current values into the sequence array
      *pSeq++ = YValue;
      XValue += SeqStep;  //Increment the values by the step sizes
      YValue += YStep;
   }
   return(0);
}

//Pops a node from the list, 
void pop_node(int& ExpNum)
{
   SAIMnode* NextNode = NULL;  //Put the address of the second node in the list here
   SAIMnode* head = ExpHeads[ExpNum];
   
   if(!head)  //Check that there is at least one node
      return;
   
   NextNode = head->pNext;  //Fill in the temp. variable
   free(head);  //Deallocated the block for the first node
   ExpHeads[ExpNum] = NextNode;  //Point the experiment head at the new first node
}

void delete_all_nodes(int& ExpNum)
{
   while(ExpHeads[ExpNum])
      pop_node(ExpNum);  //Keep popping nodes (and freeing them) until the head points to NULL
}

//Makes a new SAIMnode list, deleting any existing nodes before beginning
//Returns 0 if successful, 1 if allocation fails, 2 if seq is empty and 3 if profile is empty
int8 create_new_node(int8* pCommand)
{
   int ExpNum = (int)*pCommand++;
   int SeqNum = (int)*pCommand++;
   int AOTFNum = (int)*pCommand;
   
   if(ExpNum >= MaxExp)
   {
      output_error(2);
      return(2);
   }
   if(!SeqTails[SeqNum])  //If the sequence doesn't exist return 2
   {
      output_error(2);
      return(3);
   }
   
   delete_all_nodes(ExpNum);  //Make sure to free any existing memory
   
   SAIMnode* head = malloc(sizeof(SAIMnode));  //Allocate space for the first node
   if(!head)  //Check that the allocation was successful
   {
      output_error(2);
      return(1);
   }
   
   ExpHeads[ExpNum] = head;  //Add the address of the experiment to the list
   
   head->pSeqEnd = SeqTails[SeqNum];  //Fill in the end of the AOI sequence pointer
   head->pSeqStart = &SeqArray[SeqNum][0];  //First AOI value location
   head->pAOTF = &AOTFArray[AOTFNum][0];  //Fill in the AOTF profile pointer
   head->pNext = NULL;  //This is the first and last node
   head->pLoop = NULL;  //Always initialize to NULL
   
   return(0);
}

//Pushes a SAIMnode onto the list of the given experiment
//Returns 0 if successful, 1 if allocation fails, 2 if seq is empty and 3 if profile is empty
//Can be used to create an experiment on an empty list
int8 push_node(int8* pCommand)
{
   int ExpNum = (int)*pCommand++;
   int SeqNum = (int)*pCommand++;
   int AOTFNum = (int)*pCommand;
   
   if(ExpNum >= MaxExp)
   {
      output_error(2);
      return(2);
   }
   if(!SeqTails[SeqNum])  //If the sequence doesn't exist return 2
   {
      output_error(2);
      return(2);
   }

   SAIMnode* NewNode = malloc(sizeof(SAIMnode));  //Allocate space for the first node
   if(!NewNode)  //Check that the allocation was successful
   {
      output_error(2);
      return(1);
   }
   
   NewNode->pSeqEnd = SeqTails[SeqNum];
   NewNode->pSeqStart = &SeqArray[SeqNum][0];
   NewNode->pAOTF = &AOTFArray[AOTFNum][0];
   NewNode->pNext = ExpHeads[ExpNum];
   NewNode->pLoop = NULL;
   ExpHeads[ExpNum] = NewNode;
   
   return(0);
}

//Removes the last node in the list at ExpNum
//Returns 0 if there are nodes left, 1 if the list is now empty, and 2 if the list was empty to begin with
int8 remove_last_node(int& ExpNum)
{
   SAIMnode* head = ExpHeads[ExpNum];
   
   if(!head)  //The list is empty
      return(2);
   if(!head->pNext)  //The list only contains one node
   {
      pop_node(ExpNum);
      return(1);
   }
   
   SAIMnode* CurrNode = ExpHeads[ExpNum];  //Starting from the first node
   
   while(CurrNode->pNext->pNext)  //While the current node is not next-to-last
      CurrNode = CurrNode->pNext;  //Increment through the list
   
   free(CurrNode->pNext->pNext);  //Deallocate the last node
   CurrNode->pNext = NULL;  //The current node is now the last node
   return(0);
}

//Add a node to the end of the list
//Returns 0 if successful, 1 if allocation fails, 2 if seq is empty and 3 if profile is empty
//Can be used to create an experiment on an empty list
int8 add_last_node(int8* pCommand)
{
   int ExpNum = (int)*pCommand++;
   int SeqNum = (int)*pCommand++;
   int AOTFNum = (int)*pCommand;
   
   if(ExpNum >= MaxExp)
   {
      output_error(2);
      return(2);
   }
   if(SeqTails[SeqNum] == NULL)  //If the sequence doesn't exist return 2
   {
      output_error(2);
      return(3);
   }

   SAIMnode* NewNode = malloc(sizeof(SAIMnode));  //Allocate space for the new node
   if(!NewNode)  //Check that the allocation was successful
   {
      output_error(2);
      return(1);
   }
   
   NewNode->pSeqEnd = SeqTails[SeqNum];
   NewNode->pSeqStart = &SeqArray[SeqNum][0];
   NewNode->pAOTF = &AOTFArray[AOTFNum][0];
   NewNode->pNext = NULL;
   NewNode->pLoop = NULL;
   
   if(!ExpHeads[ExpNum])
   {
      ExpHeads[ExpNum] = NewNode;
      return(0);
   }
   
   SAIMnode* CurrNode = ExpHeads[ExpNum];
   while(CurrNode->pNext != NULL)  //We're looking for the end of the list
      CurrNode = CurrNode->pNext;  //Increment the current pointer until we find it
   CurrNode->pNext = NewNode;  //Add the new node to the end
   return(0);
}

//Counts and returns the number of nodes and steps in a given experiment
void count_steps(int8* pCommand)
{
   SAIMnode* CurrNode = ExpHeads[(int)*pCommand++];
   int NSteps = 0, NNodes = 0;
   
   while(CurrNode)
   {
      NSteps += (CurrNode->pSeqEnd - CurrNode->pSeqStart + 2) / 2;
      NNodes++;
      CurrNode = CurrNode->pNext;
   }
   *pCommand++ = make8(NNodes, 1);
   *pCommand++ = make8(NNodes, 0);
   *pCommand++ = make8(NSteps, 1);
   *pCommand++ = make8(NSteps, 0);
}

//Clears any existing loops in the given experiment
void clear_loop(int& ExpNum)
{
   if(!ExpHeads[ExpNum])
      return;
   SAIMnode* CurrNode = ExpHeads[ExpNum];
   while(CurrNode->pNext != NULL)  //As long as pLoop is NULL and pNext exists
   {
      CurrNode->pLoop = NULL;
      CurrNode = CurrNode->pNext;  //Go to the next node in the list
   }
   return;
}
  
//Adds a loop to an experiment by linking the last node to the node number passed
//in the function call.  Returns 0 if successful, 1 if the experiment doesn't
//exist and 2 if the loop step is out of bounds
int8 build_loop(int& ExpNum, int& nLoopNode)
{
   if(!ExpHeads[ExpNum])  //If the experiment doesn't exist throw an error
   {
      output_error(2);
      return(1);
   }
   clear_loop(ExpNum);  //Make sure to clear any pre-existing loops   
   SAIMnode* pCurrNode = ExpHeads[ExpNum];  //Temp storage for looking for the loop and end
   SAIMnode* pLoopNode = NULL;  //Temp storage for saving the loop address
   int nCurrNode = 0;  //Node counter
   while(nCurrNode < nLoopNode)  //Look for the node before the loop
   {
      if(pCurrNode->pNext == NULL)  //If the loop is out of bounds
      {
         output_error(2);  //Throw an error
         return(2);
      }
      pCurrNode = pCurrNode->pNext;  //Otherwise, increment the node an counter
      nCurrNode++;
   }
   pLoopNode = pCurrNode;  //The current node is the loop node
   while(pCurrNode->pNext != NULL)
      pCurrNode = pCurrNode->pNext;
   pCurrNode->pLoop = pLoopNode;  //The current node is the last in the experiment
   return(0);
}

//Begin a SAIM experiment with a given setup
void setup_experiment(ExpSetup* pSetup)
{
   int ExpNum = pSetup->ExpNum;
   ptrdiff_t StepNum = pSetup->StepNum;
   int LoopOn = pSetup->LoopOn;
   int ActivationTime = pSetup->ActivationTime;
   int ActivationIntensity = pSetup->ActivationIntensity;
   
   FIRE_OFF(); //Turn off Fire interrupt while setting up experiment
   AOTF_SHT = 0;  //AOTF global low
         
   if(!ExpHeads[ExpNum])  //Check the experiment exists
   {
      output_error(2);
      return; 
   }
   
   if(ActivationTime)  //If there is a non-zero pre-sequence activation step time
   {
      set_scan_center(CSCenter);
      set_scan_radius(CSTIRF);  //Scan TIRF
      GDAC_LOAD;  //Update the DAC registers
      WAIT_TS();
      ADAC_RS;  //Reset the AOTF to all zeros
      int8 Ch = 0;  //UV channel (405 nm on our scope)
      update_ADAC_channel(&Ch, &ActivationIntensity);  //Set the UV intensity
      ADAC_LOAD;
      do{}while(!Flags.Ts);  //Wait for the galvs to settle
      AOTF_SHT = 1;  //Begin the activation
      delay_ms(ActivationTime);
      AOTF_SHT = 0;  //End of activation
   }
   
   Flags.SAIMLoop = 0;
   if(LoopOn)
      Flags.SAIMLoop = 1;
   StartNode = ExpHeads[ExpNum];
   StartStep = StartNode->pSeqStart;
   ptrdiff_t StepCount = 0;
   do  //Find the starting step for the experiment
   {
      if(!StartNode)  //If we've run out of nodes
      {
        output_error(3);  //Throw an error and exit
        return;
      }
      if((ptrdiff_t)((StartNode->pSeqEnd - StartNode->pSeqStart) / 2) >= (StepNum - StepCount))
      {
         StartStep = StartNode->pSeqStart + 2 * StepNum;
         StepCount = StepNum;
      }
      else
      {
         StepCount += (StartNode->pSeqEnd - StartNode->pSeqStart) / 2;
         StartNode = StartNode->pNext;
       }
   }while(StepCount < StepNum);
   
   //Put the system in the appropriate condition for the first exposure
   set_scan_center(CSCenter);
   set_scan_radius(StartStep);
   GDAC_LOAD;  //Load the new values into the DAC registers
   WAIT_TS();
   update_ADAC_all(StartNode->pAOTF);  //Change the AOTF output (Ts >> update)
   ADAC_LOAD;
   LED_EXP = LED_SCN = Flags.SAIM = 1;  //Set the appropriate bits
   Flags.Paused = Flags.LastFrame = Flags.EndOfExp = 0;
   MirrorDetectorTrigger = 0;
   if(StartStep == StartNode->pSeqEnd)
   {
      Flags.LastFrame = 1;
      if((Flags.SAIMLoop == 1) && (StartNode->pLoop != NULL))
      {
         StartNode = StartNode->pLoop;
         StartStep = StartNode->pSeqStart;
      }
      else if(StartNode->pNext != NULL)
      {
         StartNode = StartNode->pNext;
         StartStep = StartNode->pSeqStart;
      }
      else
         Flags.LastFrame = Flags.EndOfExp = 1;  //This is a single frame experiment (probably just testing), so make sure the the experiment ends
   }
   else
      StartStep += 2;
   FIRE_ON();
}

void stop_experiment(void)
{
   if(Flags.Paused || Flags.SAIM)
   {
      Flags.SAIM = Flags.Paused = 0;
      LED_EXP = 0;
   }
}

/*Setup a SAIM experiment at a given step
//Command structure is:
//{CMD, ExpNum, MSBStepNum, LSBStepNum, BoolLoop, Activation MSBTime(ms), LSBTime(ms), MSBIntensity, LSBIntensity}
*/
int8 start_experiment(int8* pCommand)
{
   static ExpSetup PreviousSetup;
   if(Flags.SAIM) stop_experiment();  //This prevents segfault caused by having two experiments running
   if(!(*pCommand))
   {
      setup_experiment(&PreviousSetup);
      return 0;
   }
   pCommand++;
   PreviousSetup.ExpNum = (int)*pCommand++;
   if(!ExpHeads[PreviousSetup.ExpNum])
      return 1;
   PreviousSetup.StepNum = make16(*pCommand++, *pCommand++);
   PreviousSetup.LoopOn = (int)*pCommand++;
   PreviousSetup.ActivationTime = make16(*pCommand++, *pCommand++);
   PreviousSetup.ActivationIntensity = make16(*pCommand++, *pCommand);
   setup_experiment(&PreviousSetup);  //Run the setup
   return 0;
}

void pause_experiment(void)
{
   if(Flags.SAIM && !Flags.Paused)
   {
      Flags.SAIM = 0;
      Flags.Paused = 1;
      enable_interrupts(int_TIMER5);
   }
}

void resume_experiment(void)
{
   if(Flags.Paused)
   {
      disable_interrupts(int_TIMER5);
      LED_EXP = 1;
      Flags.Paused = 0;
      Flags.SAIM = 1;
   }
}

void restart_experiment(void)
{
   if(Flags.Paused)
   {
      Flags.Paused = 0;
      disable_interrupts(int_TIMER5);
      LED_EXP = 0;
   }
   if(Flags.SAIM)
   {
      Flags.SAIM = 0;
      LED_EXP = 0;
   }
   start_experiment(NULL);
}
