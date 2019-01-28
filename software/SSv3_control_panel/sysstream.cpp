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