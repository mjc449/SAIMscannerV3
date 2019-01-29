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

#include "pgstream.h"
#include <qapplication.h>

PGStream::PGStream(std::weak_ptr<alignercam::Camera> &cam)
    : _camera(cam)
{
}

PGStream::~PGStream()
{
   delete _showImg;
}

void PGStream::StartStream()
{
   auto cptr = _camera.lock();
   _streamRunning = true;
   int rows, cols, bitdepth;
   cptr->GetImageDimensions(rows, cols, bitdepth);
   size_t dataSz = rows * cols * bitdepth / 8;
   cv::Mat rawImg(cv::Size(cols, rows), CV_16UC1);
   cv::Mat dispImg(cv::Size(cols, rows), CV_8UC1);
   cptr->RunCapture();
   do
   {
      if (cptr->GetNextImage(dataSz, rawImg.data) == alignercam::Camera::PGCamErr::OK)
      {
         rawImg /= 255;
         rawImg.convertTo(dispImg, CV_8UC1);
         _showImg = new QImage(dispImg.data, dispImg.cols, dispImg.rows, dispImg.step, QImage::Format_Grayscale8);
         emit ImageAvailable(*_showImg);
      }
      QApplication::processEvents();
   } while (_streamRunning == true);
   emit StreamStopped();
   return;
}

void PGStream::StopStream()
{
   _streamRunning = false;
}

