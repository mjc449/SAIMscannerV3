#ifndef PGSTREAM_H
#define PGSTREAM_H

#include <QObject>
#include <memory>
#include "PGCam\AlignerCam.h"
#include "opencv2\core\core.hpp"
#include <qimage.h>

class PGStream : public QObject
{
   Q_OBJECT

public:
    PGStream(std::weak_ptr<alignercam::Camera> &cam);
    ~PGStream();

private:
   std::weak_ptr<alignercam::Camera> _camera;   
   bool _streamRunning;
   QImage *_showImg;

signals:
   void ImageAvailable(QImage img);
   void StreamStopped();

public slots:
   void StartStream();
   void StopStream();

private slots:

};


#endif //PGSTREAM_H