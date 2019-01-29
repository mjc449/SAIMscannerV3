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

#include "OneDimensionFitter.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

namespace odf
{
   class Fitter : public VFitter
   {
   public:
      Fitter()
      {
      }

      ~Fitter() override {}

      void Destroy()
      {
         delete this;
      }

      void LoadRawImage(cv::Mat img)
      {
         _rawImg = img.clone();
         _fittedImg = cv::Mat(_rawImg.cols, _rawImg.rows, CV_8UC3);
      }

      cv::Mat GetImage()
      {
         return _fittedImg.clone();
      }

      double Angle()
      {
         cv::Mat temp;
         double angle{ 0 }, theta1{ 0 }, theta2{ 0 };
         cv::Canny(_rawImg, temp, _threshold, _threshold * 2, 3);
         std::vector<cv::Vec4i> lines;
         cv::HoughLinesP(temp, lines, 1, CV_PI / 1440.0, _accumulator, _length, _gap);
         _rawImg.convertTo(_fittedImg, CV_8UC3);
         if(lines.size() > 0)
         {
            cv::Point pt1(lines[0][0], lines[0][1]), pt2(lines[0][2], lines[0][3]);
            cv::line(_fittedImg, pt1, pt2, cv::Scalar(0, 0, 255), 3, CV_AA);
            theta1 = 360.0 * atan2(lines[0][2] - lines[0][0], lines[0][3] - lines[0][1]) / (2.0 * CV_PI);
            theta1 = theta1 < 0.0 ? -theta1 : theta1;
            theta1 = theta1 > 90.0 ? 180.0 - theta1 : theta1;
            cv::putText(_fittedImg, std::to_string(theta1), cv::Point(_fittedImg.cols - 600, _fittedImg.rows - 130), cv::FONT_HERSHEY_PLAIN, 5, cv::Scalar(0, 0, 255), 2);
         }
         if(lines.size() > 1)
         {
            cv::Point pt1(lines[1][0], lines[1][1]), pt2(lines[1][2], lines[1][3]);
            cv::line(_fittedImg, pt1, pt2, cv::Scalar(0, 255, 0), 3, CV_AA);
            theta2 = 360.0 * atan2(lines[1][2] - lines[1][0], lines[1][3] - lines[1][1]) / (2.0 * CV_PI);
            theta2 = theta2 < 0.0 ? -theta2 : theta2;
            theta2 = theta2 > 90.0 ? 180 - theta2 : theta2;
            cv::putText(_fittedImg, std::to_string(theta2), cv::Point(_fittedImg.cols - 600, _fittedImg.rows - 70), cv::FONT_HERSHEY_PLAIN, 5, cv::Scalar(0, 255, 0), 2);
         }
         angle = lines.size() > 1 ? theta2 + theta1 : -360.0;
         std::string angleText = lines.size() > 1 ? std::to_string(angle) : "???";
         cv::putText(_fittedImg, angleText, cv::Point(_fittedImg.cols - 600, _fittedImg.rows - 10), cv::FONT_HERSHEY_PLAIN, 5, cv::Scalar(255, 0, 0), 2);
         std::string nLines{ std::to_string((int)lines.size()) };
         cv::putText(_fittedImg, nLines, cv::Point(10, _fittedImg.rows - 10), cv::FONT_HERSHEY_PLAIN, 5, cv::Scalar(255, 0, 0), 2);
         return angle;
      }

      void Threshold(int thVal)
      {
         _threshold = thVal;
      }

      void Accumulator(int accum)
      {
         _accumulator = accum;
      }

      void Length(int len)
      {
         _length = len;
      }

      void Gap(int gap)
      {
         _gap = gap;
      }

   private:
      cv::Mat _rawImg;
      cv::Mat _fittedImg;
      int _threshold{ 20 };
      int _accumulator{ 200 };
      int _length{ 200 };
      int _gap{ 50 };
   };

#ifdef __cplusplus
   extern "C" {
#endif
      ONEDFITTERAPI_ OneDFitter __cdecl CreateFitter()
      {
         OneDFitter p = new Fitter;
         return p;
      }
      ONEDFITTERAPI_ void __cdecl DestroyFitter(OneDFitter fitter)
      {
         fitter->Destroy();
      }
#ifdef __cplusplus
   }
#endif
}