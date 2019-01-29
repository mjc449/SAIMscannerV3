/**/////////////////////////////////////////////////////////////////////////////
//                                                                            //
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
//////////////////////////////////////////////////////////////////////////////*/


#ifndef ALIGNERCAM_H
#define ALIGNERCAM_H

#ifdef MAKE_DLL_
#define ALIGNERCAMAPI_ __declspec(dllexport)
#else
#define ALIGNERCAMAPI_ __declspec(dllimport)
#endif //MAKE_DLL_

#include <vector>
#include <string>

namespace alignercam
{
   typedef class Camera * AlignerCamera;

   class Camera
   {
   public:
      
      /** \brief Error codes*/
      enum class PGCamErr
      {
         OK,
         ALLOC_FAIL,
         NO_CAM,
         NO_IMAGE,
         BUS_ERR,
         COULDNT_CONNECT_CAM,
         COULDNT_GET_INFO,
         COULDNT_DISCONNECT,
         NO_CONFIG,
         CONFIG_FAIL,
         NO_TRIGGER_MODE,
         TRIGGER_MODE_FAIL,
         ROI_FAIL,
         START_CAP_FAIL,
         BUFFER_NOT_SET,
         CAMERA_NOT_ARMED,
         FIRE_FAIL,
         RETRIEVAL_FAIL,
         SHUTTER_FAIL,
         READ_REGISTER_FAIL,
         UNKNOWN_PIXEL_FORMAT
      };

      virtual ~Camera() {}

      virtual void Destroy() = 0;

      virtual PGCamErr ListCameras(std::vector<std::string> &camList) = 0;

      virtual PGCamErr ConnectCamera(int camIdx) = 0;

      virtual PGCamErr DisconnectCamera(void) = 0;

      virtual PGCamErr RunCapture(void) = 0;

      virtual PGCamErr GetImageDimensions(int &rows, int &cols, int &bitdepth) = 0;

      virtual PGCamErr GetNextImage(size_t buffersz, unsigned char *buffer) = 0;
   };

#ifdef __cplusplus
   extern "C" {
#endif //__cplusplus
      ALIGNERCAMAPI_ AlignerCamera __cdecl CreateCamera();
      ALIGNERCAMAPI_ void __cdecl DestroyCamera(AlignerCamera);
#ifdef __cplusplus
   }
#endif //__cplusplus
}

#endif //ALIGNERCAM_H