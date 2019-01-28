#pragma once

#include <QtWidgets/QMainWindow>
#include <qscrollarea.h>
#include "ui_ssv3controlpanel.h"
#include <memory>
#include <vector>
#include <fstream>
#include "boost/filesystem.hpp"

#include "SSv3_driver\SAIMScannerV3.h"
#include "loggwindow.h"


class SSv3ControlPanel : public QMainWindow
{
   Q_OBJECT

public:
   SSv3ControlPanel(QWidget *parent = Q_NULLPTR);
   ~SSv3ControlPanel();

   void DumpLogs(const std::exception &e);

private:
   friend class LoggWindow;
   struct Node
   {
      int sequence;
      int profile;
   };
   /////////////////////////////////////////////////////////////////////////////
   //  Variables                                                              //
   /////////////////////////////////////////////////////////////////////////////
   Ui::SSv3ControlPanelClass ui;
   QWidget *_mainWidget;
   QScrollArea *_scrollArea;
   LoggWindow *_log{ nullptr };
   boost::filesystem::path _configPath{""};
   std::shared_ptr<SSV3::Manager> _devManager{ nullptr };
   std::shared_ptr<SSV3::Controller> _device{ nullptr };
   int _nDevices{ -1 };
   int _devNumber{ -1 };
   std::wstring _devManufacturer, _devProduct, _devSerialNumber;
   std::vector<Node> _experiment;
   std::vector<std::vector<int>> _sequences;
   std::vector<std::vector<int>> _profiles;
   std::vector<int> _currentProfile = std::vector<int>(8, 0);
   std::vector<int> _previousProfile = std::vector<int>(8, 0);
   bool _loopOnOff{ false };
   unsigned short _loopTo{};
   std::vector<double> _calibrationFactor{ 4.032903e-8, 4.231488e-3, 0 };
   double _parkLocation[2]{ 0.0, 0.0 };
   bool _demoMode{ false };
   unsigned short _xCenter{ 0x7FFF }, _yCenter{ 0x7FFF }, _tirRadius{ 0x3D00 }, _phase{ 0x0400 }, _frequency{ 0x29F1 }, _yScale{ 0x7FFF };
   bool _dontUpdate{ false };
   QString _logFilename;
   int _hwVer[2]{ 0 };
   int _fwVer[2]{ 0 };
   int _drVer[2]{ 0 };
   int _swVer[2]{ 1, 2 };
   std::fstream _errStream;
   bool _errorLogging{ false };
   bool _errThrown{ false };

   ////////////////////////////////////////////////////////////////////////////
   //  Methods                                                               //
   ////////////////////////////////////////////////////////////////////////////
   void Initialize();
   void SetupCommunications();
   void SetupExperiment();
   void SetupMenus();
   void SetupActions();
   void LaserOnOff(bool);
   void FireOnOff(bool);
   void ShutterOnOff(bool);
   void SetOneLaser(int, int);
   void UpdateProfile(int, std::vector<int>);
   int DegToDac(double);
   double DacToDeg(int);
   void SetLocationPark();
   void LoadSettings();
   void SaveSettings(boost::filesystem::path);
   boost::filesystem::path SaveSettingsAs();
   void saveSettingsSlot() { SaveSettings(_configPath); }
   void saveSettingsAsSlot() { SaveSettings(""); }
   void Exit();
   void Shutdown();
   void StartLogging();
   void LogClosed();
   void StartErrorLog();
   void StopErrorLog();
   void DemoMode();
   void AboutDevice();
   void ClearAndReset();
   void AboutSoftware();
   void ParseErrors(SSV3::Controller::SSV3ERROR, const char *);

   virtual void closeEvent(QCloseEvent * event);

   /////////////////////////////////////////////////////////////////////////////
   //  Menus and actions                                                      //
   /////////////////////////////////////////////////////////////////////////////
   QMenuBar *menuBar;
   QMenu *fileMenu;
   QMenu *logMenu;
   QMenu *deviceMenu;
   QMenu *helpMenu;
   QActionGroup *logActionGroup;
   QActionGroup *errorLogActionGroup;
   QAction *newSettings;
   QAction *loadSettings;
   QAction *saveSettings;
   QAction *exit;
   QAction *startLogging;
   QAction *startErrorLogging;
   QAction *stopErrorLogging;
   QAction *demoMode;
   QAction *aboutDevice;
   QAction *clearAndReset;
   QAction *aboutSoftware;

   public slots:
   void on_calibration_sent(double);

   private slots:
   void on_connectControllerButton_clicked();
   void on_disconnectControllerButton_clicked();
   void on_refreshControllerListButton_clicked();
   void on_visorButton_clicked();
   void on_experimentList_selectionChanged();
   void on_stepAddButton_clicked();
   void on_stepUpButton_clicked();
   void on_stepDownButton_clicked();
   void on_stepDeleteButton_clicked();
   void on_clearExperimentButton_clicked();
   void on_loopButton_clicked();
   void on_startExperimentButton_clicked();
   void on_stopExperimentButton_clicked();
   void on_laserOnOffButton_clicked();
   void on_fireOnOffButton_clicked();
   void on_shutterOnOffButton_clicked();
   void on_swTriggerButton_clicked();
   void on_laser0spinbox_valueChanged() { SetOneLaser(0, ui.laser0spinbox->value()); }
   void on_laser1spinbox_valueChanged() { SetOneLaser(1, ui.laser1spinbox->value()); }
   void on_laser2spinbox_valueChanged() { SetOneLaser(2, ui.laser2spinbox->value()); }
   void on_laser3spinbox_valueChanged() { SetOneLaser(3, ui.laser3spinbox->value()); }
   void on_laser4spinbox_valueChanged() { SetOneLaser(4, ui.laser4spinbox->value()); }
   void on_laser5spinbox_valueChanged() { SetOneLaser(5, ui.laser5spinbox->value()); }
   void on_laser6spinbox_valueChanged() { SetOneLaser(6, ui.laser6spinbox->value()); }
   void on_useCurrentLasersButton_clicked();
   void on_zeroLasersButton_clicked();
   void on_newProfileButton_clicked();
   void on_addProfileButton_clicked();
   void on_updateProfileButton_clicked();
   void on_setProfileButton_clicked();
   void on_newSequenceButton_clicked();
   void on_degreesButton_clicked();
   void on_dacButton_clicked();
   void on_modifySequenceButton_clicked();
   void on_testSequenceButton_clicked();
   void on_resetButton_clicked();
   void on_usbInterfaceButton_clicked();
   void on_radiusSlider_sliderMoved();
   void on_xParkSlider_sliderMoved();
   void on_yParkSlider_sliderMoved();
   void on_radiusSpin_valueChanged();
   void on_xParkSpin_valueChanged();
   void on_yParkSpin_valueChanged();
   void on_scanButton_clicked();
   void on_centerParkButton_clicked();
   void on_locationParkButton_clicked();
   void on_tirButton_clicked();
   void on_tirSlider_sliderMoved();
   void on_xCenterSlider_sliderMoved();
   void on_yCenterSlider_sliderMoved();
   void on_phaseSlider_sliderMoved();
   void on_yScaleSlider_sliderMoved();
   void on_tirSpin_valueChanged();
   void on_xCenterSpin_valueChanged();
   void on_yCenterSpin_valueChanged();
   void on_phaseSpin_valueChanged();
   void on_yScaleSpin_valueChanged();
   void on_frequencySpin_valueChanged();
   void on_setCalibrationButton_clicked();
   void on_calibrateButton_clicked();
   void on_advancedScanGroup_clicked();
   void on_calibrator_closed();

   /////////////////////////////////////////////////////////////////////////////
   //  Menu actions                                                           //
   /////////////////////////////////////////////////////////////////////////////
   //void OpenLog();
   //void NewLog();
   //void AboutDevice();
   //void AboutSoftware();
};
