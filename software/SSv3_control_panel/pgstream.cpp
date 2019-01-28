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

