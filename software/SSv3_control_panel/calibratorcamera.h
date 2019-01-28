#ifndef CALIBRATORCAMERA_H
#define CALIBRATORCAMERA_H

#include <memory>
#include <QWidget>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlist.h>
#include <qpixmap.h>
#include <qgraphicsscene.h>
#include <QtMultimedia\qmultimedia.h>
#include <QtMultimedia\qcamerainfo.h>
#include <QtMultimedia\qcamera.h>
#include <QtMultimedia\qcameraimagecapture.h>
#include <qcameraviewfinder.h>
#include "ui_calibratorcamera.h"
#include "PGCam\AlignerCam.h"

class CalibratorDevice;
class PGStream;
class SYSStream;

class CalibratorCamera : public QWidget, public Ui::CalibratorCamera
{
   Q_OBJECT

public:
   CalibratorCamera(CalibratorDevice *cd, QWidget *parent = Q_NULLPTR);
   ~CalibratorCamera();

private:
   CalibratorDevice *_calibrator;
   QPushButton *_snapButton, *_connectButton;
   QComboBox *_cameraListComboBox;
   QGraphicsView *_imageView;
   QGraphicsPixmapItem *_pixmap;
   QImage _rawImg;
   QVBoxLayout *_mainLayout;
   QHBoxLayout *_controlsLayout;
   std::vector<QString> _cameraList;
   int _pgCamNum, _sysCamNum;
   std::shared_ptr<alignercam::Camera> _pgcam;
   std::vector<std::string> _pgCamList;
   QList<QCameraInfo> _sysCamList;
   QThread *_captureThread;
   PGStream *_pgCaptureStream;
   SYSStream *_sysCaptureStream;
   bool _camConnected{ false };

   enum CameraType
   {
      PG_CAM,
      SYS_CAM
   } _cameraType;

   void SetupLayout(void);
   void FindCameras(void);

signals:
   void StopCapture();
   void QueueCleared();
   void NewSnap(QImage);
   void Closed();

public slots:
   void on_newImage_ready(QImage);
   void ClearQueue();

private slots:
   void on_snapButton_clicked(void);
   void on_connectButton_clicked(void);

};

#endif //CALIBRATORCAMERA_H