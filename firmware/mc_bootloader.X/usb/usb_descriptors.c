/*******************************************************************************
Copyright 2016 Microchip Technology Inc. (www.microchip.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

To request to license the code under the MLA license (www.microchip.com/mla_license), 
please contact mla_licensing@microchip.com
*******************************************************************************/

/*********************************************************************
 * Descriptor specific type definitions are defined in: usbd.h
 * Configuration information is defined in: usbcfg.h
 ********************************************************************/

/** INCLUDES *******************************************************/
#include <usb.h>
#include <usb_device_hid.h>

/** CONSTANTS ******************************************************/

/* Device Descriptor */
const USB_DEVICE_DESCRIPTOR device_dsc=
{
    0x12,                   // Size of this descriptor in bytes
    USB_DESCRIPTOR_DEVICE,  // DEVICE descriptor type
    0x0200,                 // USB Spec Release Number in BCD format
    0x00,                   // Class Code
    0x00,                   // Subclass code
    0x00,                   // Protocol code
    USB_EP0_BUFF_SIZE,      // Max packet size for EP0, see usb_config.h
    0x04D8,                 // Vendor ID
    0x003C,                 // Product ID: USB HID Bootloader
    0x0100,                 // Device release number in BCD format
    0x01,                   // Manufacturer string index
    0x02,                   // Product string index
    0x00,                   // Device serial number string index
    0x01                    // Number of possible configurations
};

/* Configuration 1 Descriptor */
const uint8_t configDescriptor1[]={
    /* Configuration Descriptor */
    0x09,                           // Size of this descriptor in bytes
    USB_DESCRIPTOR_CONFIGURATION,   // CONFIGURATION descriptor type
    0x29,0x00,                  // Total length of data for this configuration 
    1,                          // Number of interfaces in this cfg
    1,                          // Index value of this configuration
    0,                          // Configuration string index
    _DEFAULT|_SELF,             // Attributes, see usb_ch9.h
    50,                         // Max power consumption (2mA each, ex: 50 = 100mA)

    /* Interface Descriptor */
    0x09,                       // Size of this descriptor in bytes
    USB_DESCRIPTOR_INTERFACE,   // INTERFACE descriptor type
    0,                          // Interface Number
    0,                          // Alternate Setting Number
    2,                          // Number of endpoints used by this interface
    HID_INTF,                   // Class code
    0,                          // Subclass code
    0,                          // Protocol code
    0,                          // Interface string index

    /* HID Class-Specific Descriptor */
    0x09,                   // Size of this descriptor in bytes
    DSC_HID,                // HID descriptor type
    0x11,0x01,              // HID Spec Release Number in BCD format (1.11)
    0x00,                   // Country Code (0x00 for Not supported)
    HID_NUM_OF_DSC,         // Number of class descriptors, see usbcfg.h
    DSC_RPT,                // Report descriptor type
    HID_RPT01_SIZE,0x00,    // Size of the report descriptor in bytes

    /* Endpoint Descriptor */
    0x07,                       //Size of this descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,    //Endpoint Descriptor
    CUSTOM_DEVICE_HID_EP | _EP_IN, //EndpointAddress and direction
    _INT,                       //Attributes
    0x40,0x00,                  //size
    0x01,                       //Interval

    /* Endpoint Descriptor */
    0x07,                       //Size of this descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,    //Endpoint Descriptor
    CUSTOM_DEVICE_HID_EP | _EP_OUT, //EndpointAddress and direction
    _INT,                       //Attributes
    0x40,0x00,                  //size
    0x01                        //Interval
};


//Language code string descriptor
const struct{uint8_t bLength;uint8_t bDscType;uint16_t string[1];}sd000={
sizeof(sd000),USB_DESCRIPTOR_STRING,{0x0409}};

//Manufacturer string descriptor
const struct{uint8_t bLength;uint8_t bDscType;uint16_t string[25];}sd001={
sizeof(sd001),USB_DESCRIPTOR_STRING,
{'M','i','c','r','o','c','h','i','p',' ',
'T','e','c','h','n','o','l','o','g','y',' ','I','n','c','.'
}};

//Product string descriptor
const struct{uint8_t bLength;uint8_t bDscType;uint16_t string[18];}sd002={
sizeof(sd002),USB_DESCRIPTOR_STRING,
{'U','S','B',' ','H','I','D',' ','B','o','o',
't','l','o','a','d','e','r'
}};

//HID class specific report descriptor - HID custom device with 64-byte IN and OUT reports
const struct{uint8_t report[HID_RPT01_SIZE];}hid_rpt01={
{
    0x06, 0x00, 0xFF,       // Usage Page = 0xFF00 (Vendor Defined Page 1)
    0x09, 0x01,             // Usage (Vendor Usage 1)
    0xA1, 0x01,             // Collection (Application)
    0x19, 0x01,             //      Usage Minimum
    0x29, 0x40,             //      Usage Maximum 	//64 input usages total (0x01 to 0x40)
    0x15, 0x00,             //      Logical Minimum (data bytes in the report may have minimum value = 0x00)
    0x26, 0xFF, 0x00, 	  	//      Logical Maximum (data bytes in the report may have maximum value = 0x00FF = unsigned 255)
    0x75, 0x08,             //      Report Size: 8-bit field size
    0x95, 0x40,             //      Report Count: Make sixty-four 8-bit fields (the next time the parser hits an "Input", "Output", or "Feature" item)
    0x81, 0x00,             //      Input (Data, Array, Abs): Instantiates input packet fields based on the above report size, count, logical min/max, and usage.
    0x19, 0x01,             //      Usage Minimum
    0x29, 0x40,             //      Usage Maximum 	//64 output usages total (0x01 to 0x40)
    0x91, 0x00,             //      Output (Data, Array, Abs): Instantiates output packet fields.  Uses same report size and count as "Input" fields, since nothing new/different was specified to the parser since the "Input" item.
    0xC0}                   // End Collection
};

//Array of configuration descriptors
const uint8_t *const USB_CD_Ptr[]=
{
    (const uint8_t *const)&configDescriptor1
};

//ROM table of pointers to the string descriptors in flash memory, as implemented in this application.
//If implementing additional string descriptors, the table needs to be expanded.
const uint8_t *const USB_SD_Ptr[]=
{
    (const uint8_t *const)&sd000,
    (const uint8_t *const)&sd001,
    (const uint8_t *const)&sd002
};

