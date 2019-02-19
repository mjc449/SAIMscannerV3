/*
Firmware for the SAIMscannerV3.0 embedded microscope controller.
Copyright 20160726 by Marshall Colville (mjc449@cornell.edu)
*/

#ifndef FIRMWARE_0_0_H
#define FIRMWARE_0_0_H


#include <p24FJ256GB210.h>

#device NESTED_INTERRUPTS=TRUE

#fuses HS
#FUSES WDT
#FUSES NOJTAG                   //JTAG disabled
#FUSES CKSFSM                   //Clock Switching is enabled, fail Safe clock monitor is enabled
#fuses PLL96MHZ
#fuses NOPROTECT

#define BRDVER_MAJOR 3
#define BRDVER_MINOR 1
#define FWVER_MAJOR 1
#define FWVER_MINOR 3

#use delay(clock=32M, crystal=20M, USB_FULL)

#define USB_CONFIG_VID 0x04D8
#define USB_CONFIG_PID 0xF11A
#define USB_STRINGS_OVERWRITTEN

char USB_STRING_DESC_OFFSET[]={0,4,12,22};


const char const USB_STRING_DESC[]={
   //string 0 - language
      4,  //length of string index
      0x03,  //descriptor type (STRING)
      0x09,0x04,  //Microsoft Defined for US-English
   //string 1 - manufacturer
      8,  //length of string index
      0x03,  //descriptor type (STRING)
      'M',0,
      'J',0,
      'C',0,
   //string 2 - product
      10,  //length of string index
      0x03,  //descriptor type (STRING)
      'S',0,
      'S',0,
      'v',0,
      '3',0,
      //string 3 - serial number
      12,
      0x03,
      '3',0,
      '0',0,
      '0',0,
      '0',0,
      '5',0
};


#define USB_CONFIG_HID_TX_SIZE 64
#define USB_CONFIG_HID_RX_SIZE 64

#include <pic24_usb.h>
#include <SSV3_usb_desc_hid.h>
#include <usb.c>
#include <stdlib.h>
#include <stddef.h>

#use DYNAMIC_MEMORY
#include <stdlibm.h>

#pin_select SCK1OUT=PIN_F3
#pin_select SDI1=PIN_F8
#pin_select SDO1=PIN_F2
#use spi(MASTER, SPI1, MODE=1, BITS=16, ENABLE=PIN_F5, stream=PIC2PIC)

#pin_select INT1=PIN_A14
#pin_select INT2=PIN_A15

//Direct control of TRIS registers to increase speed
#use FAST_IO(A)
#use FAST_IO(B)
#use FAST_IO(C)
#use FAST_IO(D)
#use FAST_IO(E)

#word Tmr1Loc=getenv("SFR:TMR1")

#bit LED_PWR=LATA.2
#bit LED_SHT=LATA.3
#bit LED_SCN=LATA.4
#bit LED_EXP=LATA.5
#bit AOTF_SHT=LATA.7
#bit FIRE_IN=PORTA.14
#bit ARM_IN=PORTA.15
#bit DO_0=LATC.13
#bit DO_1=LATC.14
#bit WAVE_MCLK=LATG.9

//Interrupt flag bits to simulate external triggers
struct  {
   unsigned int SI2C1IF:1;
   unsigned int MI2C1IF:1;
   unsigned int CMIF:1;
   unsigned int CNIF:1;
   unsigned int INT1IF:1;
   unsigned int :1;
   unsigned int IC7IF:1;
   unsigned int IC8IF:1;
   unsigned int :1;
   unsigned int OC3IF:1;
   unsigned int OC4IF:1;
   unsigned int T4IF:1;
   unsigned int T5IF:1;
   unsigned int INT2IF:1;
   unsigned int U2RXIF:1;
   unsigned int U2TXIF:1;
} MCU_IFS1;
#word MCU_IFS1 = 0x086

//Command ID defines
//0x0X are control commands
#define CMD_BLINK_PWR      0x00
#define CMD_VISOR          0x01
#define CMD_BLINK_PWR_VAR  0x02

//0x1X are scan commands
#define CMD_SET_RADIUS     0x10
#define CMD_SET_CTR        0x11
#define CMD_SET_TIRF       0x12
#define CMD_CIRCLE_SCAN    0x13
#define CMD_TIRF_SCAN      0x14
#define CMD_LOC_PARK       0x15
#define CMD_DISC_SCAN      0x16

#define CMD_DISC_SCAN_OFF  0x1E
#define CMD_CENTER_PARK    0x1F

//0x2X are DDS commands
#define CMD_SET_FREQ       0x20
#define CMD_DEFAULT_FREQ   0x21
#define CMD_SET_PHASE      0x22
#define CMD_DEFAULT_PHASE  0x23
#define CMD_MCLK_TOGGLE    0x24
#define CMD_WAVE_RS        0x25
#define CMD_WAVE_CLR_RS    0x26

//0x3X are AUX_DAC commands
#define CMD_CONST_AUX      0x30
#define CMD_MID_AUX        0x31
#define CMD_ZERO_AUX       0x32

//0x4X are AOTF commands
#define CMD_GLOBAL_HIGH    0x40
#define CMD_GLOBAL_LOW     0x41
#define CMD_CHANGE_CH      0x42
#define CMD_LOAD_PROFILE   0x43
#define CMD_OPEN_SHUTTER   0x44
#define CMD_CLOSE_SHUTTER  0x45
#define CMD_TOGGLE_OPEN    0x46
#define CMD_ADD_PROFILE    0x4C
#define CMD_AOTF_RESET     0x4F

//0x5X are interrupt commands
#define CMD_FIRE_ON        0x50
#define CMD_FIRE_OFF       0x51
#define CMD_SW_TRIGGER     0x5F

//0x6X are triggering commands

//0x7X are SC <-> SC communications

//0x8X are experiment commands
#define CMD_GET_SEQ_USB    0x80
#define CMD_DEL_SEQ        0x81
#define CMD_ADD_SEQ_LIN    0x82
#define CMD_ADD_EXP        0x83
#define CMD_ADD_NODE_START 0x84
#define CMD_ADD_NODE_END   0x85
#define CMD_ADD_LOOP       0x86
#define CMD_START_EXP      0x87
#define CMD_PAUSE_EXP      0x88
#define CMD_RESUME_EXP     0x89
#define CMD_RESTART_EXP    0x8A
#define CMD_DEL_EXP        0x8B
#define CMD_DEL_NODE_START 0x8C
#define CMD_DEL_NODE_END   0x8D
#define CMD_COUNT_STEPS    0x8E
#define CMD_STOP_EXP       0x8F

//0x9X are SimpleSAIM commands
#define CMD_LOAD_SIMPLE_HALF  0x90
#define CMD_LOAD_SIMPLE_FULL  0x91
#define CMD_DIRECTION         0x92
#define CMD_START_SIMPLE      0x93
#define CMD_START_DITHERED    0x94
#define CMD_STOP_SIMPLE       0x95
#define CMD_STEP_COUNT        0x96

//0xAX are Mirror Detector commands
#define CMD_SET_MD_RADIUS  0xA0
#define CMD_MD_ON          0xA1
#define CMD_MD_OFF         0xA2

//0xFX are special function commands
#define CMD_TS_PERIOD      0xF0
#define CMD_GET_SETTINGS   0xF1
#define CMD_GET_INFO       0xF2
#define CMD_CHECK_MEM      0xFD
#define CMD_SEND_STAT      0xFE
#define CMD_RESET_CPU      0xFF

//Macros
#define OPEN_SHUTTER DO_1 = LED_SHT = 1;  //Open the mechanical shutter
#define CLOSE_SHUTTER DO_1 = LED_SHT  = AOTF_SHT = 0;  //Close the mechanical shutter

//Inline function prototypes
#inline void FIRE_ON(void);
#inline void FIRE_OFF(void);
#inline void WAIT_TS(void);
#inline void WAIT_SWTRIGGER(int);

//Prototypes
void process_command(void);
void check_memory_exists(int8* pCommand);
void visor(void);
void output_error(int Blinks);
void initialization(void);
void report_settings(int8 *pCommand);

//Global status flags
static struct Flag_Word{
   int Ts:1;
   int SAIM:1;
   int SAIMLoop:1;
   int LastFrame:1;
   int Paused:1;
   int Fire:1;
   int Arm:1;
   int DiscScan:1;
   int EndOfExp:1;
   int SimpleSAIM:1;
   int AlwaysOpen:1;
   int UseMirrorDetector:1;
   int SWTrigger:1;
   int SWTriggerState:1;
   } Flags;


//Global Variables
static int Zero[] = {0, 0};  //Sometimes you need a variable that's zero
static int ErrBlink = 0;  //Count the number of blinks
static int ErrMSG = 0;  //The number of blinks to be output
static int TsReset = 0xF8C0;  //Timer1 reset value for ~120 us Ts period
static int ArmDelay = 0x00F0;  //Delay between Arm or Ts and next trigger

#endif //FIRMWARE_0_0_H