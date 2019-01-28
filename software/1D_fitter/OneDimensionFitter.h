#ifndef ONEDIMENSIONFITTER_H
#define ONEDIMENSIONFITTER_H

#ifdef MAKE_DLL_
#define ONEDFITTERAPI_ __declspec(dllexport)
#else
#define ONEDFITTERAPI_ __declspec(dllimport)
#endif //MAKE_DLL_

#include "opencv2/core/core.hpp"

namespace odf
{
   typedef class VFitter * OneDFitter;

   class VFitter
   {
   public:
      virtual ~VFitter() {}

      virtual void Destroy() = 0;

      virtual void LoadRawImage(cv::Mat) = 0;

      virtual cv::Mat GetImage() = 0;

      virtual double Angle() = 0;

      virtual void Threshold(int) = 0;

      virtual void Accumulator(int) = 0;

      virtual void Length(int) = 0;

      virtual void Gap(int) = 0;
   };

#ifdef __cplusplus
   extern "C" {
#endif
      ONEDFITTERAPI_ OneDFitter __cdecl CreateFitter();
      ONEDFITTERAPI_ void __cdecl DestroyFitter(OneDFitter);
#ifdef __cplusplus
   }
#endif
}

#endif //ONEDIMENSIONFITTER_H