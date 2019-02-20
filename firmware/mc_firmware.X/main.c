/*******************************************************************************
 * Copyright 2018 Marshall Colville (mjc449@cornell.edu)
 * Original Copyright 2016 Microchip Technology Inc. (www.microchip.com)
 * 
 * Derived from the PIC24FJ256GB210 Explorer bootloader example
 * Modified for the OpenMicroBoard hardware by Marshall Colville
 * 
 * 
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*******************************************************************************/

/** INCLUDES *******************************************************/
#include "system/system.h"

#include "usb/usb.h"
#include "usb/usb_device_hid.h"

#include "usb/app_device_custom_hid.h"



int main(void)
{
    SYSTEM_Initialize(SYSTEM_STATE_USB_START);

    USBDeviceInit();
    USBDeviceAttach();

    while(1)
    {
        ClrWdt();
        USB_Check_Rx();
        OtherTasks();
    }//end while
}//end main

/*******************************************************************************
 End of File
*/

