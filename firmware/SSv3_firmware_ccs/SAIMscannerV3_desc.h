
///////////////////////////////////////////////////////////////////////////
///                      SAIMscannerV3_desc.h                            ////
////                                                                   ////
//// Device / configuration descriptors FOR USB 4 channel controller   ////
//// based on CCS's HID Demo example (see ex_usb_hid.c)                ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////                                                                   ////
//// Version History:                                                  ////
////                                                                   ////
//// 01/01/07 version 1                                                ////
//// PIC18Fxx5x initial release                                        ////
//// 08/14/07 modified to transfer 6 bytes                             ////
//// 06/24/08 modified to transfer 50 bytes
//// 06/25/08 modified to transfer 54 bytes
///  07/27/09 modified to transfer 32 bytes, desc to "MPP SCANCARD"
#IFNDEF __USB_DESCRIPTORS__
#DEFINE __USB_DESCRIPTORS__
#include <usb.h>

   //////////////////////////////////////////////////////////////////
   ///
   /// HID Report. Tells HID driver how to handle and deal with
   /// received data. HID Reports can be extremely complex,
   /// see HID specifcation FOR help on writing your own.
   ///
   //////////////////////////////////////////////////////////////////
   CONST char USB_CLASS_SPECIFIC_DESC[]  =  
   {
      6, 0, 255, // Usage Page = Vendor Defined
      9, 1,  // Usage = IO device
      0xa1, 1, // Collection = Application
      0x19, 1, // Usage minimum
      0x29, 8, // Usage maximum
      0x15, 0x80, // Logical minimum (-128)
      0x25, 0x7F, // Logical maximum (127)
      0x75, 8, // Report size = 8 (bits)
      0x95, 64, // Report count = 4 bytes **** changed to be 64 bytes
      0x81, 2, // Input (Data, Var, Abs)
      0x19, 1, // Usage minimum
      0x29, 8, // Usage maximum
      0x75, 8, // Report size = 8 (bits)
      0x95, 64, // Report count = 4 bytes **** changed to be 64 bytes
      0x91, 2, // Output (Data, Var, Abs)
      0xc0  // End Collection
   };

   //IF a class has an extra descriptor not part of the config descriptor,
   // this lookup table defines where to look FOR it in the const
   // USB_CLASS_SPECIFIC_DESC[] array.
   //first element is the config number (IF your device has more than one config)
   //second element is which interface number
   //set element to 0xFFFF IF this config / interface combo doesn't exist
   const INT16 USB_CLASS_SPECIFIC_DESC_LOOKUP[USB_NUM_CONFIGURATIONS][1] =
   {
      //config 1
      //interface 0
      0
   };

   //IF a class has an extra descriptor not part of the config descriptor,
   // this lookup table defines the size of that descriptor.
   //first element is the config number (IF your device has more than one config)
   //second element is which interface number
   //set element to 0xFFFF IF this config / interface combo doesn't exist
   const INT16 USB_CLASS_SPECIFIC_DESC_LOOKUP_SIZE[USB_NUM_CONFIGURATIONS][1] =
   {
      //config 1
      //interface 0
      32
   };

   //////////////////////////////////////////////////////////////////
   ///
   ///   start config descriptor
   ///   right now CCS only supports one configuration descriptor.
   ///   the config, interface, class, and endpoint goes into this array.
   ///
   //////////////////////////////////////////////////////////////////
   #DEFINE USB_TOTAL_CONFIG_LEN 41 //config + interface + class + endpoint + endpoint (2 endpoints)
   CONST char USB_CONFIG_DESC[]  =
   {
      //IN ORDER TO COMPLY WITH WINDOWS HOSTS, THE ORDER OF THIS ARRAY MUST BE:
      // config (s)
      // interface (s)
      // class (es)
      // endpoint (s)
      //config_descriptor FOR config index 1
      USB_DESC_CONFIG_LEN, //length of descriptor size  ==1
      USB_DESC_CONFIG_TYPE, //constant CONFIGURATION (CONFIGURATION 0x02) == 2
      USB_TOTAL_CONFIG_LEN, 0, //size of all data returned FOR this config ==3, 4
      1, //number of interfaces this device supports  == 5
      0x01, //identifier for this configuration. (IF we had more than one configurations) ==6
      0x00, //index of string descriptor FOR this configuration  == 7
      0xC0, //bit 6 = 1 IF self powered, bit 5 = 1 if supports remote wakeup (we don't), bits 0 - 4 unused and bit7 = 1  == 8
      0x32, //maximum bus power required (maximum milliamperes / 2) (0x32 = 100mA)
      //interface descriptor 1
      USB_DESC_INTERFACE_LEN, //length of descriptor  = 10
      USB_DESC_INTERFACE_TYPE, //constant INTERFACE (INTERFACE 0x04)  =11
      0x00, //number defining this interface (IF we had more than one interface) ==12
      0x00, //alternate setting  == 13
      2, //number of endpoins, except 0 (pic167xx has 3, but we dont have to use all) . ==14
      0x03, //class code, 03 = HID ==15
      0x00, //subclass code //boot  == 16
      0x00, //protocol code  == 17
      0x00, //index of string descriptor FOR interface  == 18
      //class descriptor 1 (HID)
      USB_DESC_CLASS_LEN, //length of descriptor == 19
      USB_DESC_CLASS_TYPE, //descriptor type (0x21 == HID) ==20
      0x00, 0x01, //hid class release number (1.0) (try 1.10) ==21,22
      0x00, //localized country code (0 = none)  == 23
      0x01, //number of hid class descriptors that follow (1) ==24
      0x22, //report descriptor type (0x22 == HID)    ==25
      USB_CLASS_SPECIFIC_DESC_LOOKUP_SIZE[0][0], 0x00, //length of report descriptor ==26, 27
      //endpoint descriptor
      USB_DESC_ENDPOINT_LEN, //length of descriptor     == 28
      USB_DESC_ENDPOINT_TYPE, //constant ENDPOINT (ENDPOINT 0x05)  ==29
      0x81, //endpoint number and direction (0x81 = EP1 IN)  == 30
      0x03, //transfer type supported (0x03 is interrupt)  ==31
      USB_EP1_TX_SIZE, 0x00, //maximum packet size supported    ==32, 33
      10, //polling interval, in ms. (cant be smaller than 10)  == 34
      //endpoint descriptor
      USB_DESC_ENDPOINT_LEN, //length of descriptor     == 35
      USB_DESC_ENDPOINT_TYPE, //constant ENDPOINT (ENDPOINT 0x05)  ==36
      0x01, //endpoint number and direction (0x01 = EP1 OUT) ==37
      0x03, //transfer type supported (0x03 is interrupt)  ==38
      USB_EP1_RX_SIZE, 0x00, //maximum packet size supported    ==39, 40
      10 //polling interval, in ms. (cant be smaller than 10) ==41
   };

   //****** BEGIN CONFIG DESCRIPTOR LOOKUP TABLES ********
   //since we can't make pointers to constants in certain pic16s, this is an offset table to find
   // a specific descriptor in the above table.
   //NOTE: DO TO A LIMITATION OF THE CCS CODE, ALL HID INTERFACES MUST START AT 0 AND BE SEQUENTIAL
   // FOR EXAMPLE, IF YOU HAVE 2 HID INTERFACES THEY MUST BE INTERFACE 0 AND INTERFACE 1
   #define USB_NUM_HID_INTERFACES 1
   //the maximum number of interfaces seen on any config
   //for example, IF config 1 has 1 interface and config 2 has 2 interfaces you must define this as 2
   #define USB_MAX_NUM_INTERFACES 1
   //define how many interfaces there are per config. [0] is the first config, etc.
   CONST char USB_NUM_INTERFACES[USB_NUM_CONFIGURATIONS] ={1};
   //define where to find class descriptors
   //first dimension is the config number
   //second dimension specifies which interface
   //last dimension specifies which class in this interface to get, but most will only have 1 class per interface
   //IF a class descriptor is not valid, set the value to 0xFFFF
   const INT16 USB_CLASS_DESCRIPTORS[USB_NUM_CONFIGURATIONS][1][1]=
   {
      //config 1
      //interface 0
      //class 1
      18
   };

   #if (SIZEOF (USB_CONFIG_DESC) != USB_TOTAL_CONFIG_LEN)
   #error USB_TOTAL_CONFIG_LEN not defined correctly
   #endif
   //////////////////////////////////////////////////////////////////
   ///
   ///   start device descriptors
   ///
   //////////////////////////////////////////////////////////////////
   CONST char USB_DEVICE_DESC[USB_DESC_DEVICE_LEN] =
   {
      //starts of with device configuration. only one possible
      USB_DESC_DEVICE_LEN, //the length of this report == 1
      0x01, //the constant DEVICE (DEVICE 0x01) == 2
      0x10, 0x01, //usb version in bcd (pic167xx is 1.1) == 3,4
      0x00, //class code == 5
      0x00, //subclass code == 6
      0x00, //protocol code == 7
      USB_MAX_EP0_PACKET_LENGTH, //max packet size FOR endpoint 0. (SLOW SPEED SPECIFIES 8) ==8
      0x61, 0x04, //vendor id (0x04D8 is Microchip, or is it 0x0461 ??)
      0x05, 0x00, //product id ==11, 12 //don't use ffff says usb - by - example guy. oops
      0x00, 0x01, //device release number == 13, 14
      0x01, //index of string description of manufacturer. therefore we point to string_1 array (see below) == 15
      0x02, //index of string descriptor of the product == 16
      0x00, //index of string descriptor of serial number == 17
      USB_NUM_CONFIGURATIONS //number of possible configurations == 18
   };

   //////////////////////////////////////////////////////////////////
   ///
   ///   start string descriptors
   ///   String 0 is a special language string, and must be defined.  People in U.S.A. can leave this alone.
   ///
   ///   You must define the length ELSE get_next_string_character() will not see the string
   ///   Current code only supports 10 strings (0 thru 9)
   ///
   //////////////////////////////////////////////////////////////////
   //the offset of the starting location of each string.  offset[0] is the start of string 0, offset[1] is the start of string 1, etc.
   CHAR USB_STRING_DESC_OFFSET[]={0,4,12};
   char CONST USB_STRING_DESC[]=
   {
      //string 0 = S / N
      4, //length of string index
      USB_DESC_STRING_TYPE, //descriptor type 0x03 (STRING)
      0x00, 0x04, //Microsoft Defined FOR US - English
      //string 1 = Manufacturer
      8, //length of string index
      USB_DESC_STRING_TYPE, //descriptor type 0x03 (STRING)
      'M', 0,
      'J', 0,
      'C', 0,
      //string 2 = product name
      18, //length of string index
      USB_DESC_STRING_TYPE, //descriptor type 0x03 (STRING)
      'S', 0,
      'S', 0,
      'V', 0,
      '3', 0,
      '0', 0,
      '0', 0,
      ' ', 0,
      ' ', 0
   };

   #ENDIF
