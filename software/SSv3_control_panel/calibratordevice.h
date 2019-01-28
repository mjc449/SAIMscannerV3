#ifndef CALIBRATORDEVICE_H
#define CALIBRATORDEVICE_H

#include <QWidget>
#include <qlineedit.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qtablewidget.h>
#include <qlabel.h>
#include <qslider.h>
#include <QtCharts>
#include <QtCharts\qscatterseries.h>
#include <qimage.h>
#include <qgraphicsscene.h>
#include <qpixmap.h>
#include <qrubberband.h>
#include "ssv3controlpanel.h"
#include "opencv2/core/core.hpp"
#include "ui_calibratordevice.h"

class CameraLauncher;
class CalibratorCamera;

class CalibratorDevice : public QWidget, public Ui::CalibratorDevice
{
   Q_OBJECT

public:
   CalibratorDevice(SSv3ControlPanel *cp, QWidget *parent = Q_NULLPTR);
   ~CalibratorDevice();

private:
   friend class CalibratorCamera;
   SSv3ControlPanel *_controlPanel;
   QLabel *_angleLabel, *_dacLabel, *_fitLabel, *_angleValueLabel, *_fitValueLabel, *_kernelLabel, *_kernelValueLabel, *_thresholdLabel, *_thresholdValueLabel, *_accumulatorLabel, *_accumulatorValueLabel, *_lengthLabel, *_lengthValueLabel, *_gapLabel, *_gapValueLabel, *_sampleNLabel, *_targetNLabel;
   QPushButton *_imageButton, *_cameraButton, *_resetViewButton, *_sendButton, *_addButton, *_deleteButton, *_dilateButton, *_erodeButton, *_detectLinesButton;
   QSpinBox *_dacValueEdit;
   QDoubleSpinBox *_sampleNEdit, *_targetNEdit;
   QTableWidget *_dataTable;
   QScatterSeries *_dataSeries, *_correctedSeries;
   QLineSeries *_fitSeries;
   QHBoxLayout *_parentLayout, *_labelsLayout, *_valuesLayout, *_imageButtonsLayout, *_fitButtonsLayout, *_calibrationConstLayout, *_sampleNLayout, *_targetNLayout, *_imageSettingsLayout;
   QGridLayout *_settingsLayout;
   QVBoxLayout *_imageLayout, *_fitLayout, *_plotLayout, *_dilateErodeLayout;
   QValueAxis *_dataXAxis, *_dataYAxis;
   QChartView *_dataPlot;
   QGraphicsView *_imageView, *_edgesView;
   QDir _searchPath;
   CalibratorCamera *_cameraWidget;
   QSlider *_kernelSlider, *_thresholdSlider, *_accumulatorSlider, *_lengthSlider, *_gapSlider;
   bool _cameraWidgetRunning{ false };
   std::vector <std::pair<double, double>> _dataPoints;
   QRubberBand *_roiSelectionRubberband;
   QPointF _roiOrigin, _roiCorner;
   QRect _roiRect;

   QImage _rawImg, _edgesImg, _dispImg;
   cv::Mat _rawMat, _edgesMat, _dispMat;
   QGraphicsPixmapItem *_pixmap, *_edgesmap;

   int _kernel{ 3 };
   int _threshold{ 200 };
   int _accumulator{ 250 };
   int _length{ 500 };
   int _gap{ 5 };
   double _angle{ -1.0 };
   double _linConst{ 0 };
   double _sampleN{ 1.37 };
   double _targetN{ 1.00 };

   void SetupGUI(void);
   void SetupSignalsSlots(void);
   void DetectAngle(void);
   void DetectEdges(void);
   void LinearFit(void);
   virtual void resizeEvent(QResizeEvent *event);

   //Misc
   void UpdateImage(void);
   void ProcessImage(void);

signals:
   void SendCalibration(double);

public slots:
   void CameraClosed();
   void OnNewSnap(QImage);

   //SLOTS
private slots:
   void on_imageButton_clicked(void);
   void on_cameraButton_clicked(void);
   void on_addButton_clicked(void);
   void on_deleteButton_clicked(void);
   void on_sendCalibration_clicked(void);

   void on_kernelSlider_moved(void);
   void on_thresholdSlider_moved(void);
   void on_accumulatorSlider_moved(void);
   void on_lengthSlider_moved(void);
   void on_gapSlider_moved(void);

   void on_sampleN_changed(double);
   void on_targetN_changed(double);

   void on_roiSelection(QRect, QPointF, QPointF);
   void on_resetViewButton_clicked();

   void on_dilateButton_clicked(void);
   void on_erodeButton_clicked(void);
   void on_detectLinesButton_clicked(void);
};


#endif //CALIBRATORDEVICE_H