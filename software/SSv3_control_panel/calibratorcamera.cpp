#include "calibratorcamera.h"
#include "calibratordevice.h"
#include "pgstream.h"
#include "sysstream.h"

#define PGCAMERR alignercam::Camera::PGCamErr

CalibratorCamera::CalibratorCamera(CalibratorDevice *cd, QWidget *parent) :
   _pgcam(alignercam::CreateCamera(), &(alignercam::DestroyCamera))
{
   _calibrator = cd;
   setWindowTitle("Calibration Camera Live View");
   SetupLayout();
   FindCameras();
}

CalibratorCamera::~CalibratorCamera()
{
   if (_camConnected == true)
   {
      on_connectButton_clicked();
   }
   emit Closed();
}

void CalibratorCamera::SetupLayout(void)
{
   _snapButton = new QPushButton;
   _connectButton = new QPushButton;
   _cameraListComboBox = new QComboBox;
   _imageView = new QGraphicsView;
   _pixmap = new QGraphicsPixmapItem;
   _imageView->setScene(new QGraphicsScene(this));
   _imageView->scene()->addItem(_pixmap);
   _mainLayout = new QVBoxLayout;
   _controlsLayout = new QHBoxLayout;

   _snapButton->setText("Snap");
   _snapButton->setEnabled(false);
   _connectButton->setText("Connect");
   _connectButton->setCheckable(true);
   _controlsLayout->addWidget(_cameraListComboBox);
   _controlsLayout->addWidget(_connectButton);
   _controlsLayout->addWidget(_snapButton);
   _mainLayout->addWidget(_imageView);
   _mainLayout->addLayout(_controlsLayout);
   setLayout(_mainLayout);

   connect(_connectButton, SIGNAL(clicked()), SLOT(on_connectButton_clicked()));
   connect(_snapButton, SIGNAL(clicked()), SLOT(on_snapButton_clicked()));
}

void CalibratorCamera::FindCameras(void)
{
   if (_pgcam->ListCameras(_pgCamList) == PGCAMERR::OK)
      for (auto i : _pgCamList)
         _cameraList.push_back(QString::fromStdString(i));
   _pgCamNum = _pgCamList.size();
   _sysCamList = QCameraInfo::availableCameras();
   for(auto i : _sysCamList)
      _cameraList.push_back(i.description());
   _sysCamNum = _sysCamList.size();
   for (auto i : _cameraList)
      _cameraListComboBox->addItem(i);
}

void CalibratorCamera::on_connectButton_clicked(void)
{
   if (_camConnected == false)
   {
      if (_cameraListComboBox->count() == 0)
      {
         _connectButton->setChecked(false);
         return;
      }
      _pgcam->DisconnectCamera();
      
      int selectedCamIdx = _cameraListComboBox->currentIndex();
      if (selectedCamIdx < _pgCamNum)
      {
         _cameraType = PG_CAM;
         int rows, cols, bitdepth;
         _pgcam->ConnectCamera(selectedCamIdx);
         _pgcam->GetImageDimensions(rows, cols, bitdepth);
         QImage newImg(cols, rows, QImage::Format_Grayscale8);
         _pixmap->setPixmap(QPixmap::fromImage(newImg).scaled(_imageView->width(), _imageView->height(), Qt::KeepAspectRatio));
         _captureThread = new QThread;
         std::weak_ptr<alignercam::Camera> weakpgcam(_pgcam);
         _pgCaptureStream = new PGStream(weakpgcam);
         _pgCaptureStream->moveToThread(_captureThread);
         connect(_pgCaptureStream, SIGNAL(ImageAvailable(QImage)), this, SLOT(on_newImage_ready(QImage)));
         connect(_captureThread, SIGNAL(started()), _pgCaptureStream, SLOT(StartStream()));
         connect(this, SIGNAL(StopCapture()), _pgCaptureStream, SLOT(StopStream()));
         connect(_pgCaptureStream, SIGNAL(StreamStopped()), this, SLOT(ClearQueue()));
         _captureThread->start();
      }
      else if (selectedCamIdx < (_pgCamNum + _sysCamNum))
      {
         _cameraType = SYS_CAM;
         _sysCaptureStream = new SYSStream(selectedCamIdx - _pgCamNum);
         _captureThread = new QThread;
         _sysCaptureStream->moveToThread(_captureThread);
         connect(_sysCaptureStream, SIGNAL(ImageAvailable(QImage)), this, SLOT(on_newImage_ready(QImage)));
         connect(_captureThread, SIGNAL(started()), _sysCaptureStream, SLOT(StartStream()));
         connect(this, SIGNAL(StopCapture()), _sysCaptureStream, SLOT(StopStream()));
         connect(_sysCaptureStream, SIGNAL(StreamStopped()), this, SLOT(ClearQueue()));
         _captureThread->start();
      }
      else
      {
         _connectButton->setChecked(false);
         return;
      }
      _camConnected = true;
      _cameraListComboBox->setEnabled(false);
      _snapButton->setEnabled(true);
   }
   else
   {
      switch(_cameraType)
      {
      case PG_CAM:
         disconnect(_pgCaptureStream, SIGNAL(ImageAvailable(QImage)), this, SLOT(on_newImage_ready(QImage)));
         QCoreApplication::processEvents();
         emit StopCapture();
         break;
      case SYS_CAM:
         disconnect(_sysCaptureStream, SIGNAL(ImageAvailable(QImage)), this, SLOT(on_newImage_ready(QImage)));
         QCoreApplication::processEvents();
         emit StopCapture();
         break;
      default:
         break;
      }
      _camConnected = false;
      _cameraListComboBox->setEnabled(true);
      _snapButton->setEnabled(false);
   }
}

void CalibratorCamera::ClearQueue()
{
   QCoreApplication::processEvents();
   _captureThread->quit();
   _captureThread->wait();
}

void CalibratorCamera::on_snapButton_clicked(void)
{
   emit NewSnap(_rawImg);
}

void CalibratorCamera::on_newImage_ready(QImage img)
{
   _rawImg = img;
   _pixmap->setPixmap(QPixmap::fromImage(_rawImg));
   _imageView->fitInView(_imageView->scene()->sceneRect(), Qt::KeepAspectRatio);
}