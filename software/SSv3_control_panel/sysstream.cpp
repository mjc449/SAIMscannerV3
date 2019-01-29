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

#include "sysstream.h"
#include <opencv2\imgproc\imgproc.hpp>
#include <qapplication.h>
#include <qthread.h>

SYSStream::SYSStream(int camIdx) : _cameraIdx(camIdx) {};

SYSStream::~SYSStream() 
{
   if (_camera != nullptr)
      delete _camera;
   if (_imageBuffer != nullptr)
      delete _imageBuffer;
};

void SYSStream::StartStream()
{
   _camera = new cv::VideoCapture();
   _camera->open(_cameraIdx);
   if (_camera->isOpened() == false)
   {
      emit StreamFail();
      delete _camera;
      return;
   }
   int counter{ 0 };
   while (!_camera->read(_rawImg) && (counter < 100000)) {}
   if (_rawImg.empty())
   {
      emit StreamFail();
      return;
   }
   _streamRunning = true;
   QImage::Format fmt;
   if (_rawImg.channels() == 1)
      fmt = QImage::Format_Grayscale8;
   if (_rawImg.channels() == 3)
      fmt = QImage::Format_RGB888;
   do
   {
      if (_camera->read(_rawImg))
      {
         cv::cvtColor(_rawImg, _rawImg, CV_RGB2BGR);
         _imageBuffer = new QImage(_rawImg.data, _rawImg.cols, _rawImg.rows, _rawImg.step, fmt);
         emit ImageAvailable(*_imageBuffer);
      }
      QApplication::processEvents();
   } while (_streamRunning == true);
   _camera->release();
   emit StreamStopped();
}

void SYSStream::StopStream()
{
   _streamRunning = false;
}