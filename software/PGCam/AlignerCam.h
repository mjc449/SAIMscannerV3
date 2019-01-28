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