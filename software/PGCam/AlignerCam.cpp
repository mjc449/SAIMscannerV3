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

#include "AlignerCam.h"
#include "FlyCapture2.h"
#include <mutex>

namespace FC2 = FlyCapture2;

namespace alignercam
{
   void EOFCallback(FC2::Image *, const void *);

   class ACamera : public Camera
   {
   public:
      ACamera() {}

      ~ACamera() override {}

      void Destroy()
      {
         DisconnectCamera();
         delete[] _camInfos;
         delete[] _camIds;
         delete this;
      }

      PGCamErr ListCameras(std::vector<std::string> &list)
      {
         PGCamErr retval = PGCamErr::OK;
         if (_cameras.size() > 0)
         {
            _cameras.clear();
         }
         if (_camInfos != nullptr)
         {
            delete[] _camInfos;
            _camInfos = nullptr;
         }
         if (_camIds != nullptr)
         {
            delete[] _camIds;
            _camIds = nullptr;
         }

         unsigned int nCams;
         _error = _bus.GetNumOfCameras(&nCams);
         if (nCams == 0)
            return PGCamErr::NO_CAM;

         _camInfos = new FC2::CameraInfo[nCams];
         _camIds = new FC2::PGRGuid[nCams];
         if ((_camInfos == nullptr) || (_camIds == nullptr))
            return PGCamErr::ALLOC_FAIL;

         _camera = new FC2::Camera;
         for (unsigned int i = 0; i < nCams; i++)
         {
            _error = _bus.GetCameraFromIndex(i, &_camIds[i]);
            if (_error != FC2::PGRERROR_OK)
               continue;
            _error = _camera->Connect(&_camIds[i]);
            if (_error != FC2::PGRERROR_OK)
               continue;
            _error = _camera->GetCameraInfo(&_camInfos[i]);
            if (_error != FC2::PGRERROR_OK)
               continue;
            _cameras.push_back(_camInfos[i].modelName);
            _camera->Disconnect();
         }
         delete _camera;
         _camera = nullptr;

         list = _cameras;
         return retval;
      }

      PGCamErr ConnectCamera(int camNum)
      {
         PGCamErr retval = PGCamErr::OK;
         if (camNum > sizeof(_camIds))
            return PGCamErr::NO_CAM;
         if (_camera != nullptr)
            DisconnectCamera();

         if (_camInfos[camNum].interfaceType == FC2::INTERFACE_USB3)
         {
            FC2::Camera *cam = new FC2::Camera;
            if (cam == nullptr)
               return PGCamErr::ALLOC_FAIL;
            if (cam->Connect(&_camIds[camNum]) != FC2::PGRERROR_OK)
               return PGCamErr::COULDNT_CONNECT_CAM;
            FC2::Format7Info fm7info;
            bool fm7sup;
            cam->GetFormat7Info(&fm7info, &fm7sup);
            FC2::Format7ImageSettings fm7settings;
            unsigned int packetSz{ fm7info.packetSize };
            float percentage{ fm7info.percentage };
            cam->GetFormat7Configuration(&fm7settings, &packetSz, &percentage);
            fm7settings.pixelFormat = FC2::PIXEL_FORMAT_RAW16;
            cam->SetFormat7Configuration(&fm7settings, percentage);
            _camera = cam;
         }
         if (_camInfos[camNum].interfaceType == FC2::INTERFACE_GIGE)
         {
            FC2::GigECamera *cam = new FC2::GigECamera;
            if (cam == nullptr)
               return PGCamErr::ALLOC_FAIL;
            if (cam->Connect(&_camIds[camNum]) != FC2::PGRERROR_OK)
               return PGCamErr::COULDNT_CONNECT_CAM;
            FC2::GigEImageSettings settings;
            cam->GetGigEImageSettings(&settings);
            settings.pixelFormat = FC2::PIXEL_FORMAT_RAW16;
            cam->SetGigEImageSettings(&settings);
            _camera = cam;
         }

         //Set the data format to little-endian
         unsigned int regVal;
         _camera->ReadRegister(0x1048, &regVal);
         regVal &= ~(1UL << 0);
         _camera->WriteRegister(0x1048, regVal);

         FC2::Property gamma;
         gamma.type = FC2::GAMMA;
         gamma.onOff = false;
         FC2::Property gain;
         gain.type = FC2::GAIN;
         gain.autoManualMode = true;
         FC2::Property exposure;
         exposure.type = FC2::AUTO_EXPOSURE;
         exposure.autoManualMode = true;
         exposure.onOff = true;
         FC2::Property framerate;
         framerate.type = FC2::FRAME_RATE;
         framerate.autoManualMode = true;
         _camera->SetProperty(&gamma);
         _camera->SetProperty(&gain);
         _camera->SetProperty(&exposure);
         _camera->SetProperty(&framerate);
         FC2::TriggerMode triggerMode;
         _camera->GetTriggerMode(&triggerMode);
         triggerMode.onOff = false;
         _camera->SetTriggerMode(&triggerMode);
         FC2::FC2Config config;
         _camera->GetConfiguration(&config);
         config.grabMode = FC2::DROP_FRAMES;
         _camera->SetConfiguration(&config);

         _camera->StartCapture();
         _camera->RetrieveBuffer(&_image);
         _camera->StopCapture();
         _rows = _image.GetRows();
         _cols = _image.GetCols();
         _bitDepth = _image.GetBitsPerPixel();
         //Just in case
         _image.SetDefaultOutputFormat(FC2::PIXEL_FORMAT_RAW16);
#ifdef _DEBUG
         Heartbeat(false);
#endif //_DEBUG
         return retval;
      }

      PGCamErr DisconnectCamera(void)
      {
         PGCamErr retval = PGCamErr::OK;
         if (_camera == nullptr)
            return PGCamErr::OK;
         if (_camera->IsConnected() == false)
            return PGCamErr::OK;
         _camera->StopCapture();
#ifdef _DEBUG
         Heartbeat(true);
#endif //_DEBUG
         if (_camera->Disconnect() != FC2::PGRERROR_OK)
            return PGCamErr::COULDNT_DISCONNECT;
         delete _camera;
         _camera = nullptr;
         _rows = _cols = _bitDepth = 0;
         _newFrame = false;
         return retval;
      }

      PGCamErr RunCapture(void)
      {
         if (_camera->IsConnected() == false)
            return PGCamErr::NO_CAM;
         _camera->StartCapture();
         _newFrame = true;
         return PGCamErr::OK;
      }

      PGCamErr GetImageDimensions(int &rows, int &cols, int &bitDepth)
      {
         if (_camera->IsConnected() != true)
            return PGCamErr::NO_CAM;
         rows = _rows;
         cols = _cols;
         bitDepth = _bitDepth;
         return PGCamErr::OK;
      }

      PGCamErr GetNextImage(size_t bytes, unsigned char *dst)
      {
         if (_newFrame == false)
            return PGCamErr::NO_IMAGE;
         _camera->RetrieveBuffer(&_image);
         _image.Convert(&_buffer);
         std::memcpy(dst, _buffer.GetData(), bytes);
         return PGCamErr::OK;
      }

   private:
      std::vector<std::string> _cameras;
      unsigned int _camIdx;
      bool _isCamGigE;
      bool _newFrame;
      bool _owned;
      unsigned int _rows, _cols, _bitDepth;
      std::mutex _imageLock, _bufferLock;
      FC2::Image _image, _buffer;
      FC2::BusManager _bus;
      FC2::CameraBase *_camera{ nullptr };
      FC2::Error _error;
      FC2::CameraInfo *_camInfos{ nullptr };
      FC2::PGRGuid *_camIds{ nullptr };

      FC2::Error Heartbeat(bool on)
      {
         if (_camera == nullptr)
            return _error;
         if ((_isCamGigE == false) || (_camera->IsConnected() == false))
            return _error;
         PGCamErr retval = PGCamErr::OK;
         const unsigned int k_GVCPCapabilityAddr = 0x0934;
         const unsigned int k_GVCPConfigAddr = 0x0954;
         unsigned int regVal;
         FC2::GigECamera *gCamera = (FC2::GigECamera *)_camera;

         _error = gCamera->ReadGVCPRegister(k_GVCPCapabilityAddr, &regVal);
         if (_error != FC2::PGRERROR_OK)
            return _error;
         const unsigned int bitMask = 0x20000000;
         const bool canDisableHeartbeat = ((regVal & bitMask) == bitMask);

         if (canDisableHeartbeat)
         {
            _error = gCamera->ReadGVCPRegister(k_GVCPConfigAddr, &regVal);
            if (_error != FC2::PGRERROR_OK)
               return _error;

            if (on)
               regVal &= ~(1UL << 0);
            else
               regVal |= 1UL << 0;

            _error = gCamera->WriteGVCPRegister(k_GVCPConfigAddr, regVal);
         }
         return _error;
      }

      friend void EOFCallback(FC2::Image *, const void *);
   };

   void EOFCallback(FC2::Image *image, const void *newFrame)
   {
      *(bool *)newFrame = true;
   }

#ifdef __cplusplus
   extern "C" {
#endif //__cplusplus
      ALIGNERCAMAPI_ AlignerCamera __cdecl CreateCamera()
      {
         AlignerCamera p = new ACamera;
         return p;
      }
      ALIGNERCAMAPI_ void __cdecl DestroyCamera(AlignerCamera cam)
      {
         cam->Destroy();
      }
#ifdef __cplusplus
   }
#endif //__cplusplus
}
