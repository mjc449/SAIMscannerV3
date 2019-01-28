////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  SAIMScannerV3 - an open-source microscope controller providing an         //
//                  embedded solution for hardware synchronization.           //
//                  This library provides a compiler independent interface    //
//                  with the controller hardware on Windows systems.          //
//                  Many functions are Scanning Angle Interference Microscopy //
//                  (SAIM) specific, however the hardware and underlying      //
//                  functionality is designed to be versatile and applicable  //
//                  in a variety of applications.                             //
//                                                                            //
//  Copyright(c) 2018, Marshall Colville mjc449@cornell.edu                   //
//  All rights reserved.                                                      //
//                                                                            //
//  Redistribution and use in source and binary forms, with or without        //
//  modification, are permitted provided that the following conditions are    //
//  met :                                                                     //
//                                                                            //
//  1. Redistributions of source code must retain the above copyright notice, //
//  this list of conditions and the following disclaimer.                     //
//  2. Redistributions in binary form must reproduce the above copyright      //
//  notice, this list of conditions and the following disclaimer in the       //
//  documentation and/or other materials provided with the distribution.      //
//                                                                            //
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       //
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED //
//  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A           //
//  PARTICULAR PURPOSE ARE DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT OWNER   //
//  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,  //
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,       //
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR        //
//  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    //
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING      //
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS        //
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.              //
//                                                                            //
//  The views and conclusions contained in the software and documentation are //
//  those of the authors and should not be interpreted as representing        //
//  official policies, either expressed or implied, of the SAIMScannerV3      //
//  project, the Paszek Research Group, or Cornell University.                //
////////////////////////////////////////////////////////////////////////////////


#include "SAIMScannerV3.h"
#include "hidapi\hidapi.h"
#include <string.h>

typedef SSV3::Manager::SSV3MANAGER_ERROR ERR;

namespace SSV3
{
   class ScanCardManager : public Manager
   {
      struct Device
      {
         wchar_t *manufacturerString;
         wchar_t *productString;
         wchar_t *serialNumberString;
      };

   public:

      ~ScanCardManager() {}

      void Destroy()
      {
         hid_free_enumeration(_devInfo);
         hid_exit();
         delete this;
      }

      ERR Enumerate(int *nDevs)
      {
         if (hid_init() == -1)
            return ERR::SSV3MANAGER_ERROR_INIT_FAILED;
         _devInfo = hid_enumerate(1240, 61722);
         _devCount = 0; 
         hid_device_info *info = _devInfo;
         while (info != nullptr)
         {
            _devCount++;
            info = info->next;
         }
         *nDevs = _devCount;
         return ERR::SSV3MANAGER_ERROR_OK;
      }

      ERR DeviceCount(int *nDevs)
      {
         if (_devCount < 0)
            return ERR::SSV3MANAGER_ERROR_INIT_FAILED;
         *nDevs = _devCount;
         return ERR::SSV3MANAGER_ERROR_OK;
      }

      ERR GetDeviceInfo(const int dev, wchar_t *man, wchar_t *prod, wchar_t *sn)
      {
         if (_devCount < 1)
            return ERR::SSV3MANAGER_ERROR_NO_DEVICES;
         hid_device_info *device = _devInfo;
         for (int i = 0; i < dev; i++)
         {
            if (device->next == nullptr)
               return ERR::SSV3MANAGER_ERROR_DEVICE_INVALID;
            device = device->next;
         }
         size_t manlen = wcsnlen_s(device->manufacturer_string, 64);
         size_t prodlen = wcsnlen_s(device->product_string, 64);
         size_t snlen = wcsnlen_s(device->serial_number, 64);
         wcsncpy_s(man, 63, device->manufacturer_string, manlen);
         wcsncpy_s(prod, 63, device->product_string, prodlen);
         wcsncpy_s(sn, 63, device->serial_number, snlen);
         return ERR::SSV3MANAGER_ERROR_OK;
      }

      ERR RefreshDevices(int *nDevs)
      {
         _devCount = -1;
         hid_free_enumeration(_devInfo);
         return Enumerate(nDevs);
      }

   private:
      //////////////////////////////////////////////////////////////////////////
      //  Data members                                                        //
      //////////////////////////////////////////////////////////////////////////
      hid_device_info *_devInfo{ nullptr };
      int _devCount{ -1 };
   };

#ifdef __cplusplus
   extern "C" {
#endif
      SSV3API_ SSV3Manager __cdecl CreateManager()
      {
         SSV3Manager p = new ScanCardManager;
         return p;
      }
#ifdef __cplusplus
   }
#endif
}