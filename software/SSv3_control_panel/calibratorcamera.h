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