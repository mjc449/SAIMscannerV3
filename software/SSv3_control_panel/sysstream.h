#ifndef SYSSTREAM_H
#define SYSSTREAM_H

#include <memory>
#include <qobject.h>
#include <QtMultimedia\qcamera.h>
#include <QtMultimedia\qcamerainfo.h>
#include <QtMultimedia\qcameraimagecapture.h>
#include <opencv2\core\core.hpp>
#include <opencv2\videoio\videoio.hpp>
#include <opencv2\video\video.hpp>
#include <qimage.h>

class SYSStream : public QObject
{
   Q_OBJECT

public:
   SYSStream(int);
   ~SYSStream();

private:
   cv::VideoCapture *_camera;
   int _cameraIdx;
   QImage *_imageBuffer;
   bool _streamRunning{ false };
   cv::Mat _rawImg;

signals:
   void ImageAvailable(QImage img);
   void StreamStopped();
   void StreamFail();

public slots:
   void StartStream();
   void StopStream();

private slots:


};


#endif //SYSSTREAM_H