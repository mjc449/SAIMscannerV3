#include "calibratordevice.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "qfiledialog.h"
#include "calibratorcamera.h"


CalibratorDevice::CalibratorDevice(SSv3ControlPanel *cp, QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
    _controlPanel = cp;

    SetupGUI();
    SetupSignalsSlots();

}

CalibratorDevice::~CalibratorDevice()
{
   if(_cameraWidgetRunning == true)
      _cameraWidget->close();
}

void CalibratorDevice::SetupGUI(void)
{
   setWindowTitle("1D calibration");
   _parentLayout = new QHBoxLayout;
   _imageLayout = new QVBoxLayout;
   _imageButtonsLayout = new QHBoxLayout;
   _labelsLayout = new QHBoxLayout;
   _valuesLayout = new QHBoxLayout;
   _fitLayout = new QVBoxLayout;
   _fitButtonsLayout = new QHBoxLayout;
   _plotLayout = new QVBoxLayout;
   _dilateErodeLayout = new QVBoxLayout;
   _calibrationConstLayout = new QHBoxLayout;
   _imageSettingsLayout = new QHBoxLayout;
   _imageView = new QGraphicsView;
   _edgesView = new QGraphicsView;
   _angleLabel = new QLabel;
   _dacLabel = new QLabel;
   _fitLabel = new QLabel;
   _dacValueEdit = new QSpinBox;
   _angleValueLabel = new QLabel;
   _fitValueLabel = new QLabel;
   _dataTable = new QTableWidget;
   _dataSeries = new QScatterSeries;
   _correctedSeries = new QScatterSeries;
   _fitSeries = new QLineSeries;
   _dataPlot = new QChartView(new QChart);
   _dataXAxis = new QValueAxis;
   _dataYAxis = new QValueAxis;
   _imageButton = new QPushButton;
   _cameraButton = new QPushButton;
   _resetViewButton = new QPushButton;
   _sendButton = new QPushButton;
   _addButton = new QPushButton;
   _deleteButton = new QPushButton;
   _dilateButton = new QPushButton;
   _erodeButton = new QPushButton;
   _detectLinesButton = new QPushButton;
   _sampleNEdit = new QDoubleSpinBox;
   _targetNEdit = new QDoubleSpinBox;
   _sampleNLabel = new QLabel;
   _targetNLabel = new QLabel;
   _targetNLayout = new QHBoxLayout;
   _sampleNLayout = new QHBoxLayout;

   //ToolTips
   _imageView->setToolTip("Raw image");
   _dataTable->setToolTip("Calibration values");
   _dataPlot->setToolTip("Plot of calibration points");
   _imageButton->setToolTip("Load an image");
   _cameraButton->setToolTip("Connect to camera");
   _resetViewButton->setToolTip("Reset the current image to full size");
   _addButton->setToolTip("Adds the current values for DAC and angle to the data table");
   _deleteButton->setToolTip("Delete the selected data point from the table");
   _sendButton->setToolTip("Sends the current calibration values back to the main window");

   //Initial values and label text
   _dacLabel->setText("DAC value");
   _angleLabel->setText("Angle (deg)");
   _fitLabel->setText("Calibration constant: ");
   _dacValueEdit->setValue(0);
   _dacValueEdit->setMaximum(0x6FFF);
   _dacValueEdit->setMinimum(0);
   _angleValueLabel->setText("0.0");
   _fitValueLabel->setText("0.0");

   //Setup the image layout and associated buttons
   _pixmap = new QGraphicsPixmapItem;
   _edgesmap = new QGraphicsPixmapItem;
   _edgesView->setScene(new QGraphicsScene(this));
   _edgesView->scene()->addItem(_edgesmap);
   _edgesView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
   _edgesView->setDragMode(QGraphicsView::RubberBandDrag);
   _imageView->setScene(new QGraphicsScene(this));
   _imageView->scene()->addItem(_pixmap);
   _imageView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
   _imageView->setDragMode(QGraphicsView::RubberBandDrag);
   _imageButton->setText("Load File");
   _searchPath = QDir::homePath();
   _imageButtonsLayout->addWidget(_imageButton);
   _cameraButton->setText("Camera");
   _imageButtonsLayout->addWidget(_cameraButton);
   _resetViewButton->setText("Reset view");
   _imageButtonsLayout->addWidget(_resetViewButton);
   _imageLayout->addWidget(_edgesView);
   _imageLayout->addWidget(_imageView);
   _imageLayout->addLayout(_imageButtonsLayout);
   _settingsLayout = new QGridLayout;
   _kernelSlider = new QSlider;
   _kernelSlider->setMinimum(1);
   _kernelSlider->setMaximum(3);
   _kernelSlider->setOrientation(Qt::Vertical);
   _kernelSlider->setMouseTracking(true);
   _kernelSlider->setToolTip("Edge detector kernel size");
   _kernelSlider->setTracking(false);
   _kernelSlider->setValue(1);
   _thresholdSlider = new QSlider;
   _thresholdSlider->setMinimum(5);
   _thresholdSlider->setMaximum(250);
   _thresholdSlider->setOrientation(Qt::Vertical);
   _thresholdSlider->setMouseTracking(true);
   _thresholdSlider->setToolTip("Edge detector minimum threshold");
   _thresholdSlider->setTracking(false);
   _thresholdSlider->setValue(_threshold);
   _accumulatorSlider = new QSlider;
   _accumulatorSlider->setMinimum(20);
   _accumulatorSlider->setMaximum(500);
   _accumulatorSlider->setOrientation(Qt::Vertical);
   _accumulatorSlider->setMouseTracking(true);
   _accumulatorSlider->setToolTip("Hough transform accumulator");
   _accumulatorSlider->setTracking(false);
   _accumulatorSlider->setValue(_accumulator);
   _lengthSlider = new QSlider;
   _lengthSlider->setMinimum(20);
   _lengthSlider->setMaximum(500);
   _lengthSlider->setOrientation(Qt::Vertical);
   _lengthSlider->setMouseTracking(true);
   _lengthSlider->setToolTip("Minimum line length");
   _lengthSlider->setTracking(false);
   _lengthSlider->setValue(_length);
   _gapSlider = new QSlider;
   _gapSlider->setMinimum(5);
   _gapSlider->setMaximum(150);
   _gapSlider->setOrientation(Qt::Vertical);
   _gapSlider->setMouseTracking(true);
   _gapSlider->setToolTip("Maximum gap between line points");
   _gapSlider->setTracking(false);
   _gapSlider->setValue(_gap);
   _kernelLabel = new QLabel;
   _kernelLabel->setText("Kernel");
   _kernelValueLabel = new QLabel;
   _kernelValueLabel->setText(QString::number(_kernel));
   _thresholdLabel = new QLabel;
   _thresholdLabel->setText("Threshold");
   _thresholdValueLabel = new QLabel;
   _thresholdValueLabel->setText(QString::number(_threshold));
   _accumulatorLabel = new QLabel;
   _accumulatorLabel->setText("Accumulator");
   _accumulatorValueLabel = new QLabel;
   _accumulatorValueLabel->setText(QString::number(_accumulator));
   _lengthLabel = new QLabel;
   _lengthLabel->setText("Length");
   _lengthValueLabel = new QLabel;
   _lengthValueLabel->setText(QString::number(_length));
   _gapLabel = new QLabel;
   _gapLabel->setText("Gap");
   _gapValueLabel = new QLabel;
   _gapValueLabel->setText(QString::number(_gap));
   _settingsLayout->addWidget(_kernelSlider, 0, 0, Qt::AlignCenter);
   _settingsLayout->addWidget(_kernelLabel, 1, 0, Qt::AlignCenter);
   _settingsLayout->addWidget(_kernelValueLabel, 2, 0, Qt::AlignCenter);
   _settingsLayout->addWidget(_thresholdSlider, 0, 1, Qt::AlignCenter);
   _settingsLayout->addWidget(_thresholdLabel, 1, 1, Qt::AlignCenter);
   _settingsLayout->addWidget(_thresholdValueLabel, 2, 1, Qt::AlignCenter);
   _settingsLayout->addWidget(_accumulatorSlider, 0, 2, Qt::AlignCenter);
   _settingsLayout->addWidget(_accumulatorLabel, 1, 2, Qt::AlignCenter);
   _settingsLayout->addWidget(_accumulatorValueLabel, 2, 2, Qt::AlignCenter);
   _settingsLayout->addWidget(_lengthSlider, 0, 3, Qt::AlignCenter);
   _settingsLayout->addWidget(_lengthLabel, 1, 3, Qt::AlignCenter);
   _settingsLayout->addWidget(_lengthValueLabel, 2, 3, Qt::AlignCenter);
   _settingsLayout->addWidget(_gapSlider, 0, 4, Qt::AlignCenter);
   _settingsLayout->addWidget(_gapLabel, 1, 4, Qt::AlignCenter);
   _settingsLayout->addWidget(_gapValueLabel, 2, 4, Qt::AlignCenter);
   _settingsLayout->setRowStretch(0, 1);
   _settingsLayout->setRowStretch(1, 0);
   _settingsLayout->setRowStretch(2, 0);
   _dilateButton->setText("Dialate");
   _erodeButton->setText("Erode");
   _detectLinesButton->setText("Detect Lines");
   _dilateErodeLayout->addWidget(_dilateButton);
   _dilateErodeLayout->addWidget(_erodeButton);
   _dilateErodeLayout->addWidget(_detectLinesButton);
   _imageSettingsLayout->addLayout(_settingsLayout);
   _imageSettingsLayout->addLayout(_dilateErodeLayout);
   _imageSettingsLayout->setStretch(0, 8);
   _imageSettingsLayout->setStretch(1, 1);
   _imageLayout->addLayout(_imageSettingsLayout);
   _imageLayout->setStretch(0, 1);
   _imageLayout->setStretch(1, 1);
   _imageLayout->setStretch(2, 0);
   _parentLayout->addLayout(_imageLayout);

   //Setup the data table
   _dataTable->setMinimumWidth(250);
   _dataTable->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
   QList<QString> dataTableHeader{ "DAC", "Angle" };
   _dataTable->setColumnCount(2);
   _dataTable->setHorizontalHeaderLabels(dataTableHeader);
   _dataTable->verticalHeader()->setVisible(false);
   _dataTable->setColumnWidth(0, 124);
   _dataTable->setColumnWidth(1, 124);
   _dataTable->setShowGrid(false);
   _dataTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
   _dataTable->setSelectionBehavior(QTableWidget::SelectionBehavior::SelectRows);
   _fitLayout->addWidget(_dataTable);
   _labelsLayout->addWidget(_dacLabel, 1);
   _labelsLayout->addWidget(_angleLabel, 1);
   _fitLayout->addLayout(_labelsLayout);
   _valuesLayout->addWidget(_dacValueEdit, 1);
   _valuesLayout->addWidget(_angleValueLabel, 1);
   _fitLayout->addLayout(_valuesLayout);
   _addButton->setText("Add");
   _fitButtonsLayout->addWidget(_addButton);
   _deleteButton->setText("Delete");
   _fitButtonsLayout->addWidget(_deleteButton);
   _fitLayout->addLayout(_fitButtonsLayout);
   _parentLayout->addLayout(_fitLayout, 1);

   //Setup the plot of values and fit
   _dataSeries->setName("Calibration points");
   _dataSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
   _dataSeries->setMarkerSize(10.0);
   _correctedSeries->setName("Adjusted calibration points");
   _correctedSeries->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
   _correctedSeries->setMarkerSize(10.0);
   _fitSeries->append(0, 0);
   _fitSeries->append(0x6FFF, 90);
   _fitSeries->setName("LinearFit");
   _dataPlot->chart()->addSeries(_dataSeries);
   _dataPlot->chart()->addSeries(_correctedSeries);
   _dataPlot->chart()->addSeries(_fitSeries);
   _dataPlot->chart()->setMinimumSize(QSize(300, 300));
   _dataPlot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
   _dataXAxis->setRange(0, 0x6FFF);
   _dataXAxis->setTickCount(5);
   _dataXAxis->setLabelFormat("%i");
   _dataXAxis->setGridLineVisible(false);
   _dataXAxis->setTitleText("DAC");
   _dataYAxis->setRange(0, 90);
   _dataYAxis->setTickCount(7);
   _dataYAxis->setLabelFormat("%i");
   _dataYAxis->setGridLineVisible(false);
   _dataYAxis->setTitleText("Deflection (deg)");
   _dataPlot->chart()->addAxis(_dataXAxis, Qt::AlignBottom);
   _dataPlot->chart()->addAxis(_dataYAxis, Qt::AlignLeft);
   _dataSeries->attachAxis(_dataXAxis);
   _dataSeries->attachAxis(_dataYAxis);
   _correctedSeries->attachAxis(_dataXAxis);
   _correctedSeries->attachAxis(_dataYAxis);
   _fitSeries->attachAxis(_dataXAxis);
   _fitSeries->attachAxis(_dataYAxis);
   _targetNLabel->setText("Calibration Target index");
   _targetNEdit->setValue(_targetN);
   _targetNLayout->addStretch();
   _targetNLayout->addWidget(_targetNLabel);
   _targetNLayout->addWidget(_targetNEdit);
   _sampleNLabel->setText("Sample index");
   _sampleNEdit->setValue(_sampleN);
   _sampleNLayout->addStretch();
   _sampleNLayout->addWidget(_sampleNLabel);
   _sampleNLayout->addWidget(_sampleNEdit);
   _calibrationConstLayout->addStretch();
   _calibrationConstLayout->addWidget(_fitLabel);
   _calibrationConstLayout->addWidget(_fitValueLabel);
   _sendButton->setText("Send Value");
   _calibrationConstLayout->addWidget(_sendButton);
   _sendButton->setEnabled(false);
   _plotLayout->addWidget(_dataPlot);
   _plotLayout->addLayout(_sampleNLayout);
   _plotLayout->addLayout(_targetNLayout);
   _plotLayout->addLayout(_calibrationConstLayout);
   _parentLayout->addLayout(_plotLayout);
   
   _parentLayout->setStretchFactor(_imageLayout, 4);
   _parentLayout->setStretchFactor(_fitLayout, 0);
   _parentLayout->setStretchFactor(_plotLayout, 4);
   setLayout(_parentLayout);
}

void CalibratorDevice::resizeEvent(QResizeEvent *event)
{
   UpdateImage();
}

void CalibratorDevice::SetupSignalsSlots(void)
{
   connect(_imageButton, SIGNAL(clicked()), SLOT(on_imageButton_clicked()));
   connect(_cameraButton, SIGNAL(clicked()), SLOT(on_cameraButton_clicked()));
   connect(_addButton, SIGNAL(clicked()), SLOT(on_addButton_clicked()));
   connect(_deleteButton, SIGNAL(clicked()), SLOT(on_deleteButton_clicked()));
   connect(_sendButton, SIGNAL(clicked()), SLOT(on_sendCalibration_clicked()));
   connect(_kernelSlider, SIGNAL(valueChanged(int)), SLOT(on_kernelSlider_moved()));
   connect(_thresholdSlider, SIGNAL(valueChanged(int)), SLOT(on_thresholdSlider_moved()));
   connect(_accumulatorSlider, SIGNAL(valueChanged(int)), SLOT(on_accumulatorSlider_moved()));
   connect(_lengthSlider, SIGNAL(valueChanged(int)), SLOT(on_lengthSlider_moved()));
   connect(_gapSlider, SIGNAL(valueChanged(int)), SLOT(on_gapSlider_moved()));
   connect(this, SIGNAL(SendCalibration(double)), _controlPanel, SLOT(on_calibration_sent(double)));
   connect(_sampleNEdit, SIGNAL(valueChanged(double)), this, SLOT(on_sampleN_changed(double)));
   connect(_targetNEdit, SIGNAL(valueChanged(double)), this, SLOT(on_targetN_changed(double)));
   connect(_imageView, SIGNAL(rubberBandChanged(QRect, QPointF, QPointF)), this, SLOT(on_roiSelection(QRect, QPointF, QPointF)));
   connect(_edgesView, SIGNAL(rubberBandChanged(QRect, QPointF, QPointF)), this, SLOT(on_roiSelection(QRect, QPointF, QPointF)));
   connect(_resetViewButton, SIGNAL(clicked()), this, SLOT(on_resetViewButton_clicked()));
   connect(_dilateButton, SIGNAL(clicked()), this, SLOT(on_dilateButton_clicked()));
   connect(_erodeButton, SIGNAL(clicked()), this, SLOT(on_erodeButton_clicked()));
   connect(_detectLinesButton, SIGNAL(clicked()), this, SLOT(on_detectLinesButton_clicked()));
}

void CalibratorDevice::on_imageButton_clicked(void)
{
   QString filePath = QFileDialog::getOpenFileName(this, "Select image file to open", _searchPath.absolutePath(), "PNG files (*.png);; TIF files (*.tif);; JPEG (*.jpg *.jpeg)", &QString("TIF files (*.tif)"));
   if (!filePath.isEmpty()) {
      _searchPath = filePath;
      _rawMat = cv::imread(filePath.toStdString(), CV_LOAD_IMAGE_GRAYSCALE | CV_LOAD_IMAGE_ANYDEPTH);
      if (_rawMat.depth() != CV_8U)
      {
         double minVal, maxVal;
         cv::minMaxIdx(_rawMat, &minVal , &maxVal);
         _rawMat.convertTo(_rawMat, CV_8U, 255.0 / maxVal, -(minVal * 255 / maxVal));
      }
      _rawImg = QImage(_rawMat.data, _rawMat.cols, _rawMat.rows, _rawMat.step, QImage::Format::Format_Grayscale8).copy();
      _edgesMat = _rawMat.clone();
      cv::cvtColor(_rawMat, _dispMat, CV_GRAY2BGR);
      DetectEdges();
   }
}

void CalibratorDevice::on_cameraButton_clicked(void)
{
   if (_cameraWidgetRunning == false)
   {
      _cameraWidget = new CalibratorCamera(this);
      _cameraWidget->setAttribute(Qt::WA_DeleteOnClose);
      _cameraWidget->setAttribute(Qt::WA_QuitOnClose, false);
      _cameraWidget->show();
      _cameraWidgetRunning = true;
      _imageButton->setEnabled(false);
      connect(_cameraWidget, SIGNAL(NewSnap(QImage)), this, SLOT(OnNewSnap(QImage)));
      connect(_cameraWidget, SIGNAL(Closed()), this, SLOT(CameraClosed()));
   }
   else
   {
      _cameraWidget->activateWindow();
   }
}

void CalibratorDevice::CameraClosed()
{
   _imageButton->setEnabled(true);
   _cameraWidgetRunning = false;
}

void CalibratorDevice::UpdateImage(void)
{
   if (_dispMat.empty() || _edgesMat.empty())
      return;
   _edgesmap->setPixmap(QPixmap::fromImage(_edgesImg));
   _edgesView->scene()->setSceneRect(_edgesView->scene()->itemsBoundingRect());
   _edgesView->fitInView(_edgesView->scene()->sceneRect(), Qt::KeepAspectRatio);
   _pixmap->setPixmap(QPixmap::fromImage(_dispImg));
   _imageView->scene()->setSceneRect(_imageView->scene()->itemsBoundingRect());
   _imageView->fitInView(_imageView->scene()->sceneRect(), Qt::KeepAspectRatio);
}

void CalibratorDevice::on_roiSelection(QRect rect, QPointF origin, QPointF corner)
{
   if (rect.isValid())
   {
      _roiRect = rect;
      _roiOrigin = origin;
      _roiCorner = corner;
      return;
   }
   else
   {
      double sx, sy, sw, sh, vx, vy, vw, vh;
      QRectF sceneCoords = _imageView->mapToScene(_roiRect).boundingRect();
      sx = sceneCoords.left() < 0 ? 0 : sceneCoords.left();
      sy = sceneCoords.top() < 0 ? 0 : sceneCoords.top();
      sw = (sceneCoords.right() - sx) > _rawMat.cols - sx ? _rawMat.cols - sx : (sceneCoords.right() - sx);
      sh = (sceneCoords.bottom() - sy) > _rawMat.rows - sy ? _rawMat.rows - sy : (sceneCoords.bottom() - sy);
      if ((sw < 1) || (sh < 1))
         return;
      cv::Mat temp = _rawMat.clone();
      _rawMat = temp(cv::Rect(sx, sy, sw, sh)).clone();
      DetectEdges();
   }
}

void CalibratorDevice::on_resetViewButton_clicked()
{
   OnNewSnap(_rawImg);
}

void CalibratorDevice::ProcessImage(void)
{
   _angleValueLabel->setText(QString::number(_angle));
   cv::cvtColor(_dispMat, _dispMat, CV_BGR2RGB);
   _edgesImg = QImage(_edgesMat.data, _edgesMat.cols, _edgesMat.rows, _edgesMat.step, QImage::Format::Format_Grayscale8);
   _dispImg = QImage(_dispMat.data, _dispMat.cols, _dispMat.rows, _dispMat.step, QImage::Format_RGB888);
   UpdateImage();
}

void CalibratorDevice::OnNewSnap(QImage img)
{
   _rawImg = img;
   int imfmt = _rawImg.format();
   switch (_rawImg.format())
   {
   case QImage::Format::Format_Grayscale8:
      _rawMat = cv::Mat(_rawImg.height(), _rawImg.width(), CV_8UC1, _rawImg.bits(), _rawImg.bytesPerLine()).clone();
      break;
   case QImage::Format::Format_RGB888:
      _rawMat = cv::Mat(_rawImg.height(), _rawImg.width(), CV_8UC3, _rawImg.bits(), _rawImg.bytesPerLine()).clone();
      cv::cvtColor(_rawMat, _rawMat, CV_RGB2GRAY);
      break;
   default:
      return;
   }
   DetectEdges();
}

void CalibratorDevice::DetectEdges()
{
   _dispMat = cv::Mat(_rawMat.rows, _rawMat.cols, CV_8UC3);
   cv::cvtColor(_rawMat, _dispMat, CV_GRAY2BGR);
   cv::GaussianBlur(_rawMat, _edgesMat, cv::Size(15, 15), 0);
   cv::Canny(_edgesMat, _edgesMat, _threshold, _threshold * 2, _kernel);
   ProcessImage();
   return;
}

void CalibratorDevice::on_dilateButton_clicked()
{
   cv::Mat structElement = cv::getStructuringElement(cv::MORPH_DILATE, cv::Size(10,10));
   cv::dilate(_edgesMat, _edgesMat, structElement);
   ProcessImage();
   return;
}

void CalibratorDevice::on_erodeButton_clicked()
{
   cv::Mat structElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
   cv::erode(_edgesMat, _edgesMat, structElement);
   ProcessImage();
   return;
}

void CalibratorDevice::on_detectLinesButton_clicked()
{
   DetectAngle();
}

void CalibratorDevice::DetectAngle()
{
   double angle{ 0 }, theta1{ 0 }, theta2{ 0 };
   std::vector<cv::Vec4i> lines;
   cv::HoughLinesP(_edgesMat, lines, 1, CV_PI / 1440.0, _accumulator, _length, _gap);
   _dispMat = cv::Mat(_rawMat.cols, _rawMat.rows, CV_8UC3);
   cv::cvtColor(_rawMat, _dispMat, CV_GRAY2BGR);
   if (lines.size() > 0)
   {
      cv::Point pt1(lines[0][0], lines[0][1]), pt2(lines[0][2], lines[0][3]);
      cv::line(_dispMat, pt1, pt2, cv::Scalar(0, 0, 255), 3, CV_AA);
      theta1 = 360.0 * atan2(lines[0][2] - lines[0][0], lines[0][3] - lines[0][1]) / (2.0 * CV_PI);
      theta1 = theta1 < 0.0 ? -theta1 : theta1;
      theta1 = theta1 > 90.0 ? 180.0 - theta1 : theta1;
      cv::putText(_dispMat, std::to_string(theta1), cv::Point(_dispMat.cols - 600, _dispMat.rows - 130), cv::FONT_HERSHEY_PLAIN, 5, cv::Scalar(0, 0, 255), 4);
   }
   if (lines.size() > 1)
   {
      cv::Point pt1(lines[1][0], lines[1][1]), pt2(lines[1][2], lines[1][3]);
      cv::line(_dispMat, pt1, pt2, cv::Scalar(0, 255, 0), 3, CV_AA);
      theta2 = 360.0 * atan2(lines[1][2] - lines[1][0], lines[1][3] - lines[1][1]) / (2.0 * CV_PI);
      theta2 = theta2 < 0.0 ? -theta2 : theta2;
      theta2 = theta2 > 90.0 ? 180 - theta2 : theta2;
      cv::putText(_dispMat, std::to_string(theta2), cv::Point(_dispMat.cols - 600, _dispMat.rows - 70), cv::FONT_HERSHEY_PLAIN, 5, cv::Scalar(0, 255, 0), 4);
   }

   angle = lines.size() > 1 ? theta2 + theta1 : -360.0;
   std::string angleText = lines.size() > 1 ? std::to_string(angle) : "???";
   cv::putText(_dispMat, angleText, cv::Point(_dispMat.cols - 600, _dispMat.rows - 10), cv::FONT_HERSHEY_PLAIN, 5, cv::Scalar(255, 0, 0), 4);
   std::string nLines{ std::to_string((int)lines.size()) };
   cv::putText(_dispMat, nLines, cv::Point(10, _dispMat.rows - 10), cv::FONT_HERSHEY_PLAIN, 5, cv::Scalar(255, 0, 0), 4);
   _angle =  angle / 2;
   ProcessImage();
}

void CalibratorDevice::on_addButton_clicked()
{
   if (_angle < 0)
      return;
   int dacVal = _dacValueEdit->value();
   _dataTable->insertRow(_dataTable->rowCount());
   QTableWidgetItem *dacItem = new QTableWidgetItem();
   QTableWidgetItem *angleItem = new QTableWidgetItem();
   dacItem->setText(QString::number(dacVal).rightJustified(5, ' '));
   angleItem->setText(QString::number(_angle, 'f', 2));
   _dataTable->setItem(_dataTable->rowCount()-1, 0, dacItem);
   _dataTable->setItem(_dataTable->rowCount()-1, 1, angleItem);
   _dataTable->sortByColumn(0, Qt::SortOrder::DescendingOrder);
   _dataSeries->insert(dacItem->row(), QPointF(dacVal, _angle));

   _dataPoints.emplace(_dataPoints.begin() + dacItem->row(), std::pair<double, double>(dacVal, _angle));
   LinearFit();
}

void CalibratorDevice::on_deleteButton_clicked()
{
   int index = _dataTable->currentRow();
   if (index >= 0)
   {
      _dataTable->removeRow(index);
      _dataSeries->removePoints(index, 1);
      _dataTable->clearSelection();
      _dataPoints.erase(_dataPoints.begin() + index);
      LinearFit();
   }
}

void CalibratorDevice::LinearFit()
{
   double ax{}, ay{}, num{}, denom{}, sampleAngle{};
   std::vector<double> x, y;
   _correctedSeries->clear();
   for (auto i : _dataPoints)
   {
      sampleAngle = CV_2PI * i.second / 360.0;
      sampleAngle = sin(sampleAngle) * _targetN / _sampleN;
      sampleAngle = asin(sampleAngle);
      sampleAngle = sampleAngle * 360.0 / CV_2PI;
      x.emplace_back(i.first);
      y.emplace_back(sampleAngle);
      _correctedSeries->append(QPointF(i.first, sampleAngle));
   }
   _linConst = std::inner_product(x.begin(), x.end(), y.begin(), 0.0) / std::inner_product(x.begin(), x.end(), x.begin(), 0.0);
   sampleAngle = _linConst * (double)0x6FFF;
   _fitSeries->replace(1, 0x6FFF, sampleAngle);
   _fitValueLabel->setText(QString("%1").arg(_linConst, 5, 'g'));
   if (_dataPoints.size() > 0)
      _sendButton->setEnabled(true);
   else
      _sendButton->setEnabled(false);
}

void CalibratorDevice::on_sendCalibration_clicked()
{
   emit SendCalibration(_linConst);
}

void CalibratorDevice::on_kernelSlider_moved()
{
   switch (_kernelSlider->value())
   {
   case 1:
      _kernel = 3;
      break;
   case 2:
      _kernel = 5;
      break;
   case 3:
      _kernel = 7;
      break;
   default:
      return;
   }
   _kernelValueLabel->setText(QString::number(_kernel));
   DetectEdges();
}

void CalibratorDevice::on_thresholdSlider_moved()
{
   _threshold = _thresholdSlider->value();
   _thresholdValueLabel->setText(QString::number(_threshold));
   DetectEdges();
}

void CalibratorDevice::on_accumulatorSlider_moved()
{
   _accumulator = _accumulatorSlider->value();
   _accumulatorValueLabel->setText(QString::number(_accumulator));
}

void CalibratorDevice::on_lengthSlider_moved()
{
   _length = _lengthSlider->value();
   _lengthValueLabel->setText(QString::number(_length));
}

void CalibratorDevice::on_gapSlider_moved()
{
   _gap = _gapSlider->value();
   _gapValueLabel->setText(QString::number(_gap));
}

void CalibratorDevice::on_sampleN_changed(double val)
{
   _sampleN = val;
   LinearFit();
}

void CalibratorDevice::on_targetN_changed(double val)
{
   _targetN = val;
   LinearFit();
}