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

#include <sstream>
#include <functional>

#include <qtimer.h>
#include <qfiledialog.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <QCloseEvent>
#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"
#include "boost/date_time.hpp"

#include "ssv3controlpanel.h"
#include "stepadddlg.h"
#include "profileadddlg.h"
#include "addcustomdialog.h"
#include "resetdialog.h"
#include "errordialog.h"
#include "hidinterfacedialog.h"
#include "hidterminal.h"
#include "setcalibrationvaluesdialog.h"
#include "SSv3_driver\SAIMScannerV3.h"
#include "generaldialog.h"
#include "loggwindow.h"
#include "calibratordevice.h"

typedef SSV3::Controller::SSV3ERROR ERR;
typedef SSv3ControlPanel cp;
namespace fs = boost::filesystem;
namespace po = boost::program_options;
namespace pt = boost::posix_time;

#define CheckErrors(err){ParseErrors((err), __FUNCTION__);}
#define TIMESTAMP pt::second_clock::local_time().time_of_day()

SSv3ControlPanel::SSv3ControlPanel(QWidget *parent)
   : QMainWindow(parent)
{
   ui.setupUi(this);
   
   Initialize();
}

SSv3ControlPanel::~SSv3ControlPanel()
{
}

void SSv3ControlPanel::closeEvent(QCloseEvent *event)
{
   QMessageBox::StandardButton confirm = QMessageBox::question(this, tr("Close SSv3"), tr("Warning: All experimental parameters will be erased, make sure to save any settings before closing"), QMessageBox::Cancel | QMessageBox::Close, QMessageBox::Cancel);

   if (confirm != QMessageBox::Close)
      event->ignore();
   else
   {
      Shutdown();
   }
}

void cp::Shutdown()
{
   if (_errorLogging)
   {
      _errStream << TIMESTAMP << " - Shutting down software\n";
      StopErrorLog();
   }
   if (_log != nullptr)
      _log->close();
   if (_devNumber != -1)
   {
      for (int i = 0; i < 8; i++)
      {
         SetOneLaser(i, 0);
      }
      _device->CenterPark();
   }
   QApplication::quit();
}

void SSv3ControlPanel::DumpLogs(const std::exception &e)
{
   if (_log != nullptr)
   {
      _log->_textBoxStream << "\n!!!!! Exception caught !!!!!\n" << e.what();
      _log->SaveLog();
   }
   if (_errorLogging)
   {
      _errStream << "Exception caught: " << e.what() << "\nTerminating";
      _errStream.close();
   }
}

void SSv3ControlPanel::Initialize()
{
   SetupMenus();
   SetupCommunications();
   SetupExperiment();
   ui.laserPropertiesGroup->setEnabled(false);
   ui.experimentGroup->setEnabled(false);
   ui.scanGroup->setEnabled(false);
   ui.laserProfileGroup->setEnabled(false);
   ui.sequenceGroup->setEnabled(false);
   ui.resetButton->setEnabled(false);
   ui.usbInterfaceButton->setEnabled(false);
   ui.degreesButton->blockSignals(true);
   int max = 0x8000;
   ui.radiusSlider->setRange(0, max);
   ui.xParkSlider->setRange(-max, max);
   ui.yParkSlider->setRange(-max, max);
   ui.radiusSlider->setSingleStep(0x80);
   ui.xParkSlider->setSingleStep(0x80);
   ui.yParkSlider->setSingleStep(0x80);
   ui.radiusSlider->setTickInterval(0x800);
   ui.xParkSlider->setTickInterval(0x800);
   ui.yParkSlider->setTickInterval(0x800);
   double maxd = DacToDeg(max);
   ui.radiusSpin->setRange(0, maxd);
   ui.xParkSpin->setRange(-maxd, maxd);
   ui.yParkSpin->setRange(-maxd, maxd);
}

//Checks the controller communications for errors
//Sets a flag if there was an error
void SSv3ControlPanel::ParseErrors(ERR error, const char *fun)
{
   if (_demoMode)
   {
      _errThrown = false;
      return;
   }
   std::string string;
   const wchar_t *response = _device->HidError();
   if (response != NULL)
   {
      _errThrown = true;
      std::wstring wstr(response);
      string = std::string(wstr.begin(), wstr.end());
      ui.controllerErrorLabel->setStyleSheet("QLabel {background-color : yellow; color : red; }");
      ui.controllerErrorLabel->setText(QString::fromStdString(string));
      if (_errorLogging)
      {
         _errStream << TIMESTAMP << " Hid error in function " << fun << " - " << string << "\n";
         _errStream.flush();
      }
   }
   const char * devError = _device->GetLastError();
   if (devError != NULL)
   {
      _errThrown = true;
      if (_errorLogging)
      {
         _errStream << TIMESTAMP << " SSv3 device error in function " << fun << " - " << devError << "\n";
         _errStream.flush();
      }
   }
   switch (error)
   {
   case ERR::SSV3ERROR_OK:
      string = "OK";
      break;
   case ERR::SSV3ERROR_NO_DEVICES_FOUND:
      string = "No devices found";
      break;
   case ERR::SSV3ERROR_DEVICE_UNAVAILABLE:
      string = "Device unavailable";
      break;
   case ERR::SSV3ERROR_NO_RESPONSE:
      string = "No response";
      break;
   case ERR::SSV3ERROR_UNEXPECTED_RETURN:
      string = "Unexpected return value";
      break;
   case ERR::SSV3ERROR_XMIT_FAIL:
      string = "Transmit failed";
      break;
   case ERR::SSV3ERROR_READ_TOO_SHORT:
      string = "Short response";
      break;
   case ERR::SSV3ERROR_EXCITATION_FULL:
      string = "Excitation list full";
      break;
   case ERR::SSV3ERROR_ALLOC_FAIL:
      string = "Memory allocation failed";
      break;
   case ERR::SSV3ERROR_EXCITATION_PROFILE_OUT_OF_RANGE:
      string = "Invalid excitation profile";
      break;
   case ERR::SSV3ERROR_EXCITATION_PROFILE_DOESNT_EXIST:
      string = "Missing excitation profile";
      break;
   case ERR::SSV3ERROR_SEQUENCES_FULL:
      string = "Sequence list full";
      break;
   case ERR::SSV3ERROR_SEQUENCE_ALLOCATION_FAIL:
      string = "Memory allocation failed";
      break;
   case ERR::SSV3ERROR_SEQUENCE_LENGTH_ZERO:
      string = "Sequence empty";
      break;
   case ERR::SSV3ERROR_SEQUENCE_DOESNT_EXIST:
      string = "Missing angle sequence";
      break;
   case ERR::SSV3ERROR_SEQUENCE_LOAD_FAILED:
      string = "Sequence upload failed";
      break;
   case ERR::SSV3ERROR_NO_EXPERIMENT:
      string = "Experiment empty";
      break;
   case ERR::SSV3ERROR_INVALID_LOOP:
      string = "Loop invalid";
      break;
   case ERR::SSV3ERROR_STEP_OUTSIDE_EXPERIMENT_RANGE:
      string = "Invalid experiment step";
      break;
   case ERR::SSV3ERROR_COULDNT_OPEN_ERR_LOG:
      string = "Could not open error log";
      break;
   default:
      string = "Unknown error";
      break;
   }
   //Only return ok if hidapi, device and api are error free
   if (error == ERR::SSV3ERROR_OK && !_errThrown)
   {
      _errThrown = false;
      ui.controllerErrorLabel->setStyleSheet("QLabel {background-color : white; color : green; }");
      ui.controllerErrorLabel->setText(QString::fromStdString(string));
      return;
   }
   else
   {
      _errThrown = true;
      ui.controllerErrorLabel->setStyleSheet("QLabel {background-color : yellow; color : red; }");
      ui.controllerErrorLabel->setText(QString::fromStdString(string));
      if (_errorLogging)
      {
         _errStream << TIMESTAMP << " SSV3 API error in function " << fun << " - " << string << "\n";
         _errStream.flush();
      }
      return;
   }
}

void cp::SetupMenus()
{
   SetupActions();

   fileMenu = menuBar->addMenu(tr("&File"));
   fileMenu->addAction(saveSettings);
   fileMenu->addAction(newSettings);
   fileMenu->addAction(loadSettings);
   fileMenu->addSeparator();
   fileMenu->addAction(exit);
   saveSettings->setEnabled(false);
   newSettings->setEnabled(false);
   loadSettings->setEnabled(false);

   logMenu = menuBar->addMenu(tr("&Logging"));
   logMenu->addAction(startLogging);
   logMenu->addSeparator();
   logMenu->addAction(startErrorLogging);
   logMenu->addAction(stopErrorLogging);

   stopErrorLogging->setEnabled(false);

   deviceMenu = menuBar->addMenu(tr("&Device"));
   //deviceMenu->addAction(clearAndReset);
   //deviceMenu->addSeparator();
   deviceMenu->addAction(demoMode);
   deviceMenu->addSeparator();
   deviceMenu->addAction(aboutDevice);

   helpMenu = menuBar->addMenu(tr("&Help"));
   helpMenu->addAction(aboutSoftware);

   this->setMenuBar(menuBar);
}

void cp::SetupActions()
{
   menuBar = new QMenuBar();
   saveSettings = new QAction(tr("&Save settings"), this);
   saveSettings->setStatusTip(tr("Save the current settings sheet"));
   connect(saveSettings, &QAction::triggered, this, &cp::saveSettingsSlot);
   newSettings = new QAction(tr("&Save settings as..."), this);
   newSettings->setStatusTip(tr("Save the current settings in a new settings sheet"));
   connect(newSettings, &QAction::triggered, this, &cp::saveSettingsAsSlot);
   loadSettings = new QAction(tr("&Load settings sheet"), this);
   loadSettings->setStatusTip(tr("Load an existing settings sheet into device memory"));
   connect(loadSettings, &QAction::triggered, this, &cp::LoadSettings);
   exit = new QAction(tr("&Exit"), this);
   exit->setStatusTip(tr("Shuts down the controller and exits"));
   connect(exit, &QAction::triggered, this, &cp::Exit);
   startLogging = new QAction(tr("&Experiment log"));
   connect(startLogging, &QAction::triggered, this, &cp::StartLogging);
   startErrorLogging = new QAction(tr("&Start error Logging"));
   connect(startErrorLogging, &QAction::triggered, this, &cp::StartErrorLog);
   stopErrorLogging = new QAction(tr("&Stop error logging"));
   connect(stopErrorLogging, &QAction::triggered, this, &cp::StopErrorLog);
   demoMode = new QAction(tr("&Demo mode"));
   connect(demoMode, &QAction::triggered, this, &cp::DemoMode);
   demoMode->setCheckable(true);
   aboutDevice = new QAction(tr("&About SSv3"));
   connect(aboutDevice, &QAction::triggered, this, &cp::AboutDevice);
   //clearAndReset = new QAction(tr("&Reset all"));
   //connect(clearAndReset, &QAction::triggered, this, &cp::ClearAndReset);
   aboutSoftware = new QAction(tr("&About SSv3ControlPanel"));
   connect(aboutSoftware, &QAction::triggered, this, &cp::AboutSoftware);
}

void cp::SaveSettings(fs::path p)
{
   if (p.empty())
      p = SaveSettingsAs();
   if (p.empty())
      return;
   if (p.extension() != ".cfg")
      p.append(".cfg");
   if (fs::exists(p) && (!fs::is_regular_file(p) && p.extension() != ".cfg"))
   {
      QMessageBox::StandardButton retry = QMessageBox::question(this, tr("Invalid file"), tr("The file specified is not a valid config file"), QMessageBox::Retry | QMessageBox::Cancel, QMessageBox::Retry);
      if (retry == QMessageBox::Retry)
         SaveSettings("");
      else
         return;
   }
   std::ofstream fileStream;
   fileStream.open(p.string(), std::fstream::out);
   if (!fileStream.is_open())
   {
      std::string msg{ strerror(errno) };
      QMessageBox::critical(this, tr("Error opening config file"), QString::fromStdString(msg));
      return;
   }
   _configPath = p;
   fileStream << "# SSv3 control panel configuration\n"
      << "calibrationConstants=" << _calibrationFactor[0] << "\n"
      << "calibrationConstants=" << _calibrationFactor[1] << "\n"
      << "calibrationConstants=" << _calibrationFactor[2] << "\n"
      << "xCenter=" << (int)_xCenter << "\n"
      << "yCenter=" << (int)_yCenter << "\n"
      << "tirRadius=" << (int)_tirRadius << "\n"
      << "phase=" << (int)_phase << "\n"
      << "frequency=" << (int)_frequency << "\n"
      << "yScale=" << (int)_yScale << "\n";
   fileStream.close();
}

fs::path cp::SaveSettingsAs()
{
   QFileDialog fileDialog;
   QString filter = "Config files (*.cfg)";
   QString path = "./";
   QString fileName = fileDialog.getSaveFileName(this, tr("Save current settings as..."), path, filter, &filter);
   fs::path p = fileName.toStdString();
   return p;
}

void cp::LoadSettings()
{
   po::options_description opts("Configuration options");
   opts.add_options()
      ("calibrationConstants", po::value<std::vector<double>>()->multitoken(), "values used to convert Deg<->DAC")
      ("xCenter", po::value<unsigned short>()->default_value(0x7FFF), "scan center x")
      ("yCenter", po::value<unsigned short>()->default_value(0x7FFF), "scan center y")
      ("tirRadius", po::value<unsigned short>()->default_value(0x4000), "TIR scan radius in DAC")
      ("phase", po::value<unsigned short>()->default_value(0x400), "x/y output offset")
      ("frequency", po::value<unsigned short>()->default_value(0x29F1), "scan circular frequency in DAC")
      ("yScale", po::value<unsigned short>()->default_value(0x7FFF), "y-galvo scale offset");

   QString filter = "Config files (*.cfg)";
   fs::path p = QFileDialog::getOpenFileName(this, tr("Open settings file..."), "./", filter, &filter).toStdString();
   if (p.empty())
      return;
   std::ifstream fStream;
   fStream.open(p.string());
   if(!fStream.is_open())
   {
      std::string msg{ strerror(errno) };
      QMessageBox::critical(this, tr("Error opening config file"), QString::fromStdString(msg));
      return;
   }

   po::variables_map vm;
   po::store(po::parse_config_file(fStream, opts), vm);
   po::notify(vm);
   ui.centerParkButton->setChecked(true);
   on_centerParkButton_clicked();
   bool adv = ui.advancedScanGroup->isEnabled();
   ui.advancedScanGroup->setEnabled(true);
   _calibrationFactor = vm["calibrationConstants"].as<std::vector<double>>();
   ui.xCenterSpin->setValue(vm["xCenter"].as<unsigned short>());
   ui.yCenterSpin->setValue(vm["yCenter"].as<unsigned short>());
   ui.tirSpin->setValue(vm["tirRadius"].as<unsigned short>());
   ui.phaseSpin->setValue(vm["phase"].as<unsigned short>());
   ui.frequencySpin->setValue(vm["frequency"].as<unsigned short>());
   ui.yScaleSpin->setValue(vm["yScale"].as<unsigned short>());
   if (adv == false)
      ui.advancedScanGroup->setEnabled(false);
}

void cp::Exit()
{
   this->close();
}

void cp::StartLogging()
{
   _log = new LoggWindow(this);
   _log->setAttribute(Qt::WA_DeleteOnClose);
   _log->setAttribute(Qt::WA_QuitOnClose, false);
   _log->show();
   startLogging->setEnabled(false);
   connect(_log, &LoggWindow::destroyed, [=] {_log = nullptr; });
}

void cp::StartErrorLog()
{
   if (_device)
      CheckErrors(_device->DetailedErrorReporting(true))
      if (_errThrown)
      {
         _errThrown = false;
         return;
      }
   if (_errStream.is_open())
      return;
   stopErrorLogging->setEnabled(true);
   startErrorLogging->setEnabled(false);
   pt::ptime now = pt::second_clock::local_time();
   fs::path p("./errlog");
   if (!fs::exists(p) || !fs::is_directory(p))
      fs::create_directory(p);
   std::stringstream s;
   s << now.date() << "_" << now.time_of_day().hours() << "-" << now.time_of_day().minutes() << "-" << now.time_of_day().seconds() << "_err.log";
   p /= s.str();
   _errStream.open(p.string(), std::ofstream::out);
   if (!_errStream.is_open())
   {
      ui.controllerErrorLabel->setText(tr("Couldn't open error log"));
      ui.controllerErrorLabel->setStyleSheet("QLabel {background-color : yellow; color : red; }");
      return;
   }
   _errStream << "SSv3 control panel version " << _swVer[0] << "." << _swVer[1] << "\n";
   _errStream << "Error logging opened at " << now.time_of_day() << " on " << now.date() << "\n";
   if (_devNumber != -1)
      _errStream << "Connected to controller S/N" << std::string(_devSerialNumber.begin(), _devSerialNumber.end()) << " Hardware version " << _hwVer[0] << "." << _hwVer[1] << " Firmware version " << _fwVer[0] << "." << _fwVer[1] << " Driver version " << _drVer[0] << "." << _drVer[1] << "\n";
   _errStream.flush();
   _errorLogging = true;
}

void cp::StopErrorLog()
{
   if (_device)
      CheckErrors(_device->DetailedErrorReporting(false))
      if (_errThrown)
      {
         _errThrown = false;
         return;
      }
   if (_errStream.is_open())
   {
      _errStream << "Error logging stopped at " << TIMESTAMP;
      _errStream.close();
   }
   startErrorLogging->setEnabled(true);
   stopErrorLogging->setEnabled(false);
}

void cp::DemoMode()
{
   if (demoMode->isChecked())
      _demoMode = true;
   else
      _demoMode = false;
   on_refreshControllerListButton_clicked();
}

void cp::AboutDevice()
{
   GeneralDialog dialog;
   int hw1{ _hwVer[0] }, hw2{ _hwVer[1] }, fw1{ _fwVer[0] }, fw2{ _fwVer[1] };
   std::stringstream s;
   s << "SSv3 controller is an embedded hardware synchronization" << std::endl
      << "platform for prototype super resoltion optical" << std::endl
      << "microscopy. Currently it is targeted for Scanning Angle" << std::endl
      << "Interference Microscopy (SAIM) using azimuthal beam" << std::endl
      << "scanning. The hardware has a wide array of capabilities" << std::endl
      << "and was developed with flexibility and expandability" << std::endl
      << "as major project goals." << std::endl
      << std::endl
      << "Copyright(c) 2018, Marshall Colville mjc449@cornell.edu" << std::endl
      << "All rights reserved." << std::endl
      << std::endl
      << "Redistribution and use in source and binary forms," << std::endl
      << "with or without modification, are permitted provided" << std::endl
      << "that the following conditions are met :" << std::endl
      << std::endl
      << "1. Redistributions of source code must retain the" << std::endl
      << "above copyright notice, this list of conditions and" << std::endl
      << "the following disclaimer." << std::endl 
      << std::endl
      << "2. Redistributions in binary form must reproduce the" << std::endl
      << "above copyright notice, this list of conditions and" << std::endl
      << "the following disclaimer in the documentation and/or" << std::endl
      << "other materials provided with the distribution." << std::endl
      << std::endl
      << "THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND" << std::endl
      << "CONTRIBUTORS \"AS IS\" AND ANY EXPRESS OR IMPLIED" << std::endl
      << "WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED" << std::endl
      << "WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A" << std::endl
      << "PARTICULAR PURPOSE ARE DISCLAIMED.IN NO EVENT SHALL" << std::endl
      << "THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY" << std::endl
      << "DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR" << std::endl
      << "CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO," << std::endl
      << "PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF" << std::endl
      << "USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)" << std::endl
      << "HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER" << std::endl
      << "IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING" << std::endl
      << "NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE" << std::endl
      << "USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE" << std::endl
      << "POSSIBILITY OF SUCH DAMAGE." << std::endl
      << std::endl
      << "Hardware revision: " << hw1 << "." << hw2 << std::endl
      << "Firmware revision: " << fw1 << "." << fw2;
   dialog.SetText(s.str());
   dialog.setWindowTitle(tr("About SSv3 device"));
   dialog.SetButtonText("Ok");
   dialog.update();
   if (dialog.exec()) {}
}

void cp::ClearAndReset()
{
   GeneralDialog dialog;
   dialog.SetText("All devices will be reset and any settings in the control panel will be lost.\n\nDo you wish to proceed?");
   dialog.SetButtonText("Ok");
   dialog.AddCancel();
   if (dialog.exec())
   {
      _device->Reset();
      _device.reset();
      _devManager.reset();
      ui.controllerTable->setRowCount(0);
      _experiment.clear();
      _sequences.clear();
      _profiles.clear();
      _currentProfile.clear();
      _previousProfile.clear();
      _loopOnOff = false;
      _parkLocation[0] = 0.0;
      _parkLocation[1] = 0.0;
      ui.laser0spinbox->setValue(0);
      ui.laser1spinbox->setValue(0);
      ui.laser2spinbox->setValue(0);
      ui.laser3spinbox->setValue(0);
      ui.laser4spinbox->setValue(0);
      ui.laser5spinbox->setValue(0);
      ui.laser6spinbox->setValue(0);
      ui.laserProfileTable->setRowCount(0);
      ui.sequenceTable->setRowCount(0);
      ui.experimentList->clear();
      ui.degreesButton->setChecked(true);
      ui.radiusSpin->setValue(0.00);
      ui.xParkSpin->setValue(0.00);
      ui.yParkSpin->setValue(0.00);
      Initialize();
   };
}

void cp::AboutSoftware()
{
   GeneralDialog dialog;
   int sw1{ _swVer[0] }, sw2{ _swVer[1] };
   std::stringstream s;
   s << "SSv3 controller is an embedded hardware synchronization" << std::endl
      << "platform for prototype super resoltion optical" << std::endl
      << "microscopy. Currently it is targeted for Scanning Angle" << std::endl
      << "Interference Microscopy (SAIM) using azimuthal beam" << std::endl
      << "scanning. The hardware has a wide array of capabilities" << std::endl
      << "and was developed with flexibility and expandability" << std::endl
      << "as major project goals." << std::endl
      << std::endl
      << "Copyright(c) 2018, Marshall Colville mjc449@cornell.edu" << std::endl
      << "All rights reserved." << std::endl
      << std::endl
      << "Redistribution and use in source and binary forms," << std::endl
      << "with or without modification, are permitted provided" << std::endl
      << "that the following conditions are met :" << std::endl
      << std::endl
      << "1. Redistributions of source code must retain the" << std::endl
      << "above copyright notice, this list of conditions and" << std::endl
      << "the following disclaimer." << std::endl
      << std::endl
      << "2. Redistributions in binary form must reproduce the" << std::endl
      << "above copyright notice, this list of conditions and" << std::endl
      << "the following disclaimer in the documentation and/or" << std::endl
      << "other materials provided with the distribution." << std::endl
      << std::endl
      << "THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND" << std::endl
      << "CONTRIBUTORS \"AS IS\" AND ANY EXPRESS OR IMPLIED" << std::endl
      << "WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED" << std::endl
      << "WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A" << std::endl
      << "PARTICULAR PURPOSE ARE DISCLAIMED.IN NO EVENT SHALL" << std::endl
      << "THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY" << std::endl
      << "DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR" << std::endl
      << "CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO," << std::endl
      << "PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF" << std::endl
      << "USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)" << std::endl
      << "HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER" << std::endl
      << "IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING" << std::endl
      << "NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE" << std::endl
      << "USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE" << std::endl
      << "POSSIBILITY OF SUCH DAMAGE." << std::endl
      << std::endl
      << "Software version: " << sw1 << "." << sw2;
   dialog.SetText(s.str());
   dialog.setWindowTitle(tr("About SSv3 Control Panel"));
   dialog.SetButtonText("Ok");
   dialog.update();
   if (dialog.exec()) {};
}

void SSv3ControlPanel::SetupCommunications()
{
   ui.controllerTable->horizontalHeaderItem(0)->setText("Manufacturer");
   ui.controllerTable->horizontalHeaderItem(0)->setText("Product");
   ui.controllerTable->horizontalHeaderItem(0)->setText("Serial Number");
   ui.controllerTable->verticalHeader()->setVisible(false);
   ui.controllerTable->setSelectionBehavior(QAbstractItemView::SelectRows);
   ui.controllerStatusLabel->setText(QString("Disconnected"));
   ui.controllerStatusLabel->setStyleSheet("QLabel {background-color : white; color : red; }");
   ui.connectControllerButton->setEnabled(true);
   ui.disconnectControllerButton->setEnabled(false);
   ui.visorButton->setEnabled(false);
   _devManager = std::shared_ptr<SSV3::Manager>(SSV3::CreateManager(), std::mem_fn(&(SSV3::Manager::Destroy)));
   _devManager->Enumerate(&_nDevices);
   for (int i = 0; i < _nDevices; i++)
   {
      wchar_t sn[64], mn[64], pd[64];
      _devManager->GetDeviceInfo(i, mn, pd, sn);
      ui.controllerTable->insertRow(i);
      ui.controllerTable->setItem(i, 0, new QTableWidgetItem(QString::fromWCharArray(mn)));
      ui.controllerTable->setItem(i, 1, new QTableWidgetItem(QString::fromWCharArray(pd)));
      ui.controllerTable->setItem(i, 2, new QTableWidgetItem(QString::fromWCharArray(sn)));
   }
   if (_nDevices < 1)
      demoMode->trigger();
   ui.controllerTable->selectRow(0);
}

void SSv3ControlPanel::on_refreshControllerListButton_clicked()
{

   if (_device)
   {
      _devNumber = 0;
      ui.controllerTable->setRowCount(1);
      std::wstringstream s;
      s << _devSerialNumber << "*";
      ui.controllerTable->setItem(0, 0, new QTableWidgetItem(QString::fromStdWString(_devManufacturer)));
      ui.controllerTable->setItem(0, 1, new QTableWidgetItem(QString::fromStdWString(_devProduct)));
      ui.controllerTable->setItem(0, 2, new QTableWidgetItem(QString::fromWCharArray(s.str().c_str())));
   }
   else
      ui.controllerTable->setRowCount(0);

   if (_demoMode && !_device)
   {
      ui.controllerTable->insertRow(0);
      ui.controllerTable->setItem(0, 0, new QTableWidgetItem(QString("MJC")));
      ui.controllerTable->setItem(0, 1, new QTableWidgetItem(QString("SSv3 Demo")));
      ui.controllerTable->setItem(0, 2, new QTableWidgetItem(QString("00000")));
      _nDevices = 1;
      return;
   }
   _devManager->RefreshDevices(&_nDevices);
   for (int i = 0; i < _nDevices; i++)
   {
      wchar_t mn[64], pd[64], sn[64];
      _devManager->GetDeviceInfo(i, mn, pd, sn);
      int currentRow = ui.controllerTable->rowCount();
      ui.controllerTable->insertRow(currentRow);
      ui.controllerTable->setItem(currentRow, 0, new QTableWidgetItem(QString::fromWCharArray(mn)));
      ui.controllerTable->setItem(currentRow, 1, new QTableWidgetItem(QString::fromWCharArray(pd)));
      ui.controllerTable->setItem(currentRow, 2, new QTableWidgetItem(QString::fromWCharArray(sn)));
   }
}

void SSv3ControlPanel::on_connectControllerButton_clicked()
{
   if (ui.controllerTable->selectedItems().count() == 0)
      return;
   if (_nDevices == 0)
      return;
   if (_demoMode)
   {
      _devNumber = 0;
      _devManufacturer = std::wstring(L"MJC");
      _devProduct = std::wstring(L"SSv3 Demo");
      _devSerialNumber = std::wstring(L"00000");
      _device.reset();
      _device = std::shared_ptr<SSV3::Controller>(SSV3::CreateDemoDevice(), std::mem_fn(&(SSV3::Controller::Destroy)));
   }
   else
   {
      wchar_t mn[64], pd[64], sn[64];
      _devNumber = ui.controllerTable->currentRow();
      _devManager->GetDeviceInfo(_devNumber, mn, pd, sn);
      _device.reset();
      _device = std::shared_ptr<SSV3::Controller>(SSV3::CreateDeviceFromSN(sn), std::mem_fn(&(SSV3::Controller::Destroy)));
      _devManufacturer = std::wstring(mn);
      _devProduct = std::wstring(pd);
      _devSerialNumber = std::wstring(sn);
   }
   if (_errorLogging)
   {
      CheckErrors(_device->DetailedErrorReporting(true));
   }
   CheckErrors(_device->Initialize());
   if (_errThrown)
   {
      ui.controllerErrorLabel->setStyleSheet("QLabel {background-color : yellow; color : red; }");
      ui.controllerErrorLabel->setText(tr("Device failed initialization"));
      if (_errorLogging)
      {
         _errStream << TIMESTAMP << "SSV3 API error in function " << __FUNCTION__ << " - Device failed initialization" << "\n";
         _errStream.flush();
      }
      _errThrown = false;
      on_disconnectControllerButton_clicked();
      return;
   }
   CheckErrors(_device->QueryInternalSettings(&_xCenter, &_yCenter, &_tirRadius, &_phase, &_frequency));
   if (_errThrown)
   {
      ui.controllerErrorLabel->setStyleSheet("QLabel {background-color : yellow; color : red; }");
      ui.controllerErrorLabel->setText(tr("Couldn't retrieve device settings"));
      if (_errorLogging)
      {
         _errStream << TIMESTAMP << "SSV3 API error in function " << __FUNCTION__ << " - Couldn't retrieve device settings" << "\n";
         _errStream.flush();
      }
      _errThrown = false;
   }
   saveSettings->setEnabled(true);
   newSettings->setEnabled(true);
   loadSettings->setEnabled(true);
   _dontUpdate = true;
   ui.xCenterSpin->setValue(_xCenter);
   ui.xCenterSlider->setValue(_xCenter);
   ui.yCenterSpin->setValue(_yCenter);
   ui.yCenterSlider->setValue(_yCenter);
   ui.tirSpin->setValue(_tirRadius);
   ui.tirSlider->setValue(_tirRadius);
   ui.phaseSpin->setValue(_phase);
   ui.phaseSlider->setValue(_phase);
   ui.frequencySpin->setValue(_frequency * 1100 / 0x2e23);
   std::wstringstream s;
   s << "Connected to controller S/N: " << _devSerialNumber;
   ui.controllerStatusLabel->setText(QString::fromWCharArray(s.str().c_str()));
   ui.controllerStatusLabel->setStyleSheet("QLabel {background-color : white; color : green; }");
   s.str(std::wstring());
   s << _devSerialNumber << "*";
   ui.controllerTable->setItem(_devNumber, 2, new QTableWidgetItem(QString::fromWCharArray(s.str().c_str())));
   ui.disconnectControllerButton->setEnabled(true);
   ui.visorButton->setEnabled(true);
   ui.connectControllerButton->setEnabled(false);
   ui.laserPropertiesGroup->setEnabled(true);
   ui.experimentGroup->setEnabled(true);
   ui.scanGroup->setEnabled(true);
   ui.centerParkButton->setChecked(true);
   on_centerParkButton_clicked();
   ui.laserProfileGroup->setEnabled(true);
   ui.sequenceGroup->setEnabled(true);
   ui.resetButton->setEnabled(true);
   ui.usbInterfaceButton->setEnabled(true);
   _dontUpdate = false;
   unsigned char hw1, hw2, fw1, fw2;
   CheckErrors(_device->QueryDevVer(&hw1, &hw2, &fw1, &fw2));
   if (_errThrown)
   {
      ui.controllerErrorLabel->setStyleSheet("QLabel {background-color : yellow; color : red; }");
      ui.controllerErrorLabel->setText(tr("Couldn't retrieve version info"));
      if (_errorLogging)
      {
         _errStream << TIMESTAMP << "SSV3 API error in function " << __FUNCTION__ << " - Couldn't retrieve version info" << "\n";
         _errStream.flush();
      }
      _errThrown = false;
   }
   _hwVer[0] = hw1;
   _hwVer[1] = hw2;
   _fwVer[0] = fw1;
   _fwVer[1] = fw2;
   if (_errorLogging)
   {
      _errStream << "Connected to controller S/N" << std::string(_devSerialNumber.begin(), _devSerialNumber.end()) << " Hardware version " << _hwVer[0] << "." << _hwVer[1] << " Firmware version " << _fwVer[0] << "." << _fwVer[1] << " Driver version " << _drVer[0] << "." << _drVer[1] << "\n";
      _errStream.flush();
   }
   if (_log != nullptr)
      _log->_textBoxStream << "\n>>" << pt::to_simple_string(TIMESTAMP) << " - Controller connected\n";
}

void SSv3ControlPanel::on_disconnectControllerButton_clicked()
{
   if (!_device)
      return;
   on_stopExperimentButton_clicked();
   if (ui.testSequenceButton->isChecked())
   {
      ui.testSequenceButton->setChecked(false);
      on_testSequenceButton_clicked();
   }
   for (int i = 0; i < 8; i++)
      SetOneLaser(i, 0);
   _device.reset();
   _devNumber = -1;
   saveSettings->setEnabled(false);
   newSettings->setEnabled(false);
   loadSettings->setEnabled(false);
   _sequences.clear();
   _profiles.clear();
   _experiment.clear();
   ui.controllerStatusLabel->setText(QString("Disconnected"));
   ui.controllerStatusLabel->setStyleSheet("QLabel {background-color : white; color : red; }");
   ui.controllerErrorLabel->setText(QString(""));
   ui.controllerErrorLabel->setStyleSheet("QLabel {background-color : white; color : green; }");
   ui.controllerTable->setItem(_devNumber, 2, new QTableWidgetItem(QString::fromStdWString(_devSerialNumber)));
   on_refreshControllerListButton_clicked();
   ui.disconnectControllerButton->setEnabled(false);
   ui.visorButton->setEnabled(false);
   ui.connectControllerButton->setEnabled(true);
   ui.laserPropertiesGroup->setEnabled(false);
   ui.experimentGroup->setEnabled(false);
   ui.startExperimentButton->setEnabled(false);
   ui.scanGroup->setEnabled(false);
   ui.laserProfileGroup->setEnabled(false);
   ui.sequenceGroup->setEnabled(false);
   ui.resetButton->setEnabled(false);
   ui.usbInterfaceButton->setEnabled(false);
   ui.experimentList->clear();
   ui.sequenceTable->setRowCount(0);
   ui.laserProfileTable->setRowCount(0);
   _hwVer[0] = _hwVer[1] = _fwVer[0] = _fwVer[1] = 0;
   if (_errorLogging)
   {
      _errStream << "Disconnected from controller.\n";
      _errStream.flush();
   }
   if (_log != nullptr)
      _log->_textBoxStream << "\n>>" << pt::to_simple_string(TIMESTAMP) << " - Controller disconnected\n";
}

void SSv3ControlPanel::on_visorButton_clicked()
{
   if (!_device)
      return;
   CheckErrors(_device->Visor());
   if (_errThrown)
      _errThrown = false;
}

void SSv3ControlPanel::SetupExperiment()
{
   ui.stepUpButton->setEnabled(false);
   ui.stepDownButton->setEnabled(false);
   ui.stepDeleteButton->setEnabled(false);
   ui.loopButton->setEnabled(false);
   ui.clearExperimentButton->setEnabled(false);
   ui.startExperimentButton->setEnabled(false);
   ui.stopExperimentButton->setEnabled(false);
   connect(ui.experimentList, SIGNAL(itemSelectionChanged()), SLOT(on_experimentList_selectionChanged()));
}

void SSv3ControlPanel::on_experimentList_selectionChanged()
{
   if (ui.experimentList->selectedItems().size() != 0)
   {
      ui.stepUpButton->setEnabled(true);
      ui.stepDownButton->setEnabled(true);
      ui.stepDeleteButton->setEnabled(true);
      ui.loopButton->setEnabled(true);
   }
   else
   {
      ui.stepUpButton->setEnabled(false);
      ui.stepDownButton->setEnabled(false);
      ui.stepDeleteButton->setEnabled(false);
      ui.loopButton->setEnabled(false);
   }
}

void SSv3ControlPanel::on_stepAddButton_clicked()
{
   stepadddlg dialog(this);
   dialog.stepSpinBox->setMaximum(_experiment.size() + 1);
   int selection = ui.experimentList->currentRow();
   selection = selection == 0 ? 1 : selection;
   dialog.stepSpinBox->setValue(selection);
   selection = ui.laserProfileTable->currentRow() + 1;
   selection = selection < 1 ? 1 : selection;
   dialog.profileSpinBox->setValue(selection);
   selection = ui.sequenceTable->currentRow() + 1;
   selection = selection < 1 ? 1 : selection;
   dialog.sequenceSpinBox->setValue(selection);
   if (_sequences.empty() || _profiles.empty())
   {
      dialog.sequenceSpinBox->setEnabled(false);
      dialog.profileSpinBox->setEnabled(false);
      dialog.stepOkButton->setEnabled(false);
      dialog.stepAddLabel->setText("Must have defined at least 1 sequence and 1 profile.");
   }
   else
   {
      dialog.sequenceSpinBox->setMaximum(_sequences.size());
      dialog.profileSpinBox->setMaximum(_profiles.size());
   }
   if (dialog.exec())
   {
      int step = dialog.stepSpinBox->value() - 1;
      int seq = dialog.sequenceSpinBox->value() - 1;
      int prof = dialog.profileSpinBox->value() - 1;
      int position = step;
      std::stringstream s;
      s << "Sequence " << seq + 1 << ", Profile " << prof + 1;
      QString stepAsString = QString::fromStdString(s.str());
      QListWidgetItem *item = new QListWidgetItem;
      item->setText(stepAsString);
      if (_loopOnOff && _loopTo >= position)
         _loopTo++;
      ui.experimentList->insertItem(position, item);
      Node node;
      node.sequence = seq;
      node.profile = prof;
      std::vector<Node>::iterator expItr{ _experiment.begin() };
      _experiment.insert(expItr + position, node);
      if (_experiment.size() > 0)
         ui.startExperimentButton->setEnabled(true);
   }
}

void SSv3ControlPanel::on_stepDeleteButton_clicked()
{
   QListWidgetItem *item = ui.experimentList->currentItem();
   int row = ui.experimentList->row(item);
   std::vector<Node>::iterator expItr{ _experiment.begin() };
   _experiment.erase(expItr + row);
   if (_loopTo == row)
   {
      _loopTo = 0;
      _loopOnOff = false;
   }
   else if (_loopTo > row)
   {
      _loopTo--;
   }
   delete ui.experimentList->takeItem(row);
   if (ui.experimentList->count() > 0)
      if (row = 0)
         ui.experimentList->setCurrentRow(0);
      else
         ui.experimentList->setCurrentRow(row - 1);
   else
   {
      ui.clearExperimentButton->setEnabled(false);
      ui.startExperimentButton->setEnabled(false);
   }
}

void SSv3ControlPanel::on_stepUpButton_clicked()
{
   QListWidgetItem *item = ui.experimentList->currentItem();
   int row = ui.experimentList->row(item);
   if (row == 0)
      return;
   std::vector<Node>::iterator expItr{ _experiment.begin() + row };
   std::iter_swap(expItr, expItr - 1);
   if (_loopTo == row)
      _loopTo--;
   else if (_loopTo == (row - 1))
      _loopTo++;
   ui.experimentList->insertItem(row - 1, ui.experimentList->takeItem(row));
   ui.experimentList->setCurrentItem(item);
}

void SSv3ControlPanel::on_stepDownButton_clicked()
{
   QListWidgetItem *item = ui.experimentList->currentItem();
   int row = ui.experimentList->row(item);
   if (row + 1 == ui.experimentList->count())
      return;
   std::vector<Node>::iterator expItr{ _experiment.begin() + row };
   std::iter_swap(expItr, expItr + 1);
   if (_loopTo == row)
      _loopTo++;
   else if (_loopTo == (row + 1))
      _loopTo--;
   ui.experimentList->insertItem(row + 1, ui.experimentList->takeItem(row));
   ui.experimentList->setCurrentItem(item);
}

void SSv3ControlPanel::on_clearExperimentButton_clicked()
{
   ui.experimentList->clear();
   _experiment.clear();
   ui.clearExperimentButton->setEnabled(false);
   ui.loopButton->setEnabled(false);
   ui.startExperimentButton->setEnabled(false);
   _loopTo = 0;
   _loopOnOff = false;
}

void SSv3ControlPanel::on_loopButton_clicked()
{
   if (_loopOnOff == false)
   {
      QListWidgetItem *item = ui.experimentList->currentItem();
      QString name = item->text();
      name += "*";
      item->setText(name);
      _loopTo = ui.experimentList->row(item);
      _loopOnOff = true;
   }
   else
   {
      QListWidgetItem *item = ui.experimentList->item(_loopTo);
      QString name = item->text();
      std::string newName = name.toStdString();
      newName.pop_back();
      item->setText(QString::fromStdString(newName));
      _loopTo = 0;
      _loopOnOff = false;
   }
}

void SSv3ControlPanel::on_startExperimentButton_clicked()
{
   if (!_device)
   {
      ui.controllerStatusLabel->setStyleSheet("QLabel {background-color : red; color : yellow; }");
      QTimer::singleShot(1000, [this]() {ui.controllerStatusLabel->setStyleSheet("QLabel { background-color : white; color : red; }"); });
      return;
   }
   if (_experiment.empty())
   {
      return;
   }
   else
   {
      CheckErrors(_device->ClearExperiment());
      if (_errThrown)
      {
         _errThrown = false;
         return;
      }
   }
   //Load the steps from front to back
   for (auto i : _experiment)
      CheckErrors(_device->AddExperimentStep((unsigned char)i.sequence, (unsigned char)i.profile));
   if (_errThrown)
   {
      _errThrown = false;
      return;
   }
   //Build a loop if necessary
   if (_loopOnOff)
      CheckErrors(_device->Loop(_loopOnOff, _loopTo));
   if (_errThrown)
   {
      _errThrown = false;
      return;
   }
   CheckErrors(_device->StartExperiment());
   if (_errThrown)
   {
      _errThrown = false;
      return;
   }
   ui.stepAddButton->setEnabled(false);
   ui.stepUpButton->setEnabled(false);
   ui.stepDownButton->setEnabled(false);
   ui.stepDeleteButton->setEnabled(false);
   ui.clearExperimentButton->setEnabled(false);
   ui.loopButton->setEnabled(false);
   ui.stopExperimentButton->setEnabled(true);
   ui.experimentList->blockSignals(true);
   ui.startExperimentButton->setEnabled(false);
   ui.laserPropertiesGroup->setEnabled(false);
   ui.laserProfileGroup->setEnabled(false);
   ui.sequenceGroup->setEnabled(false);
   ui.scanGroup->setEnabled(false);

   //Update the log if it's running
   if (_log != nullptr)
   {
      _log->_textBox->moveCursor(QTextCursor::End);
      _log->_textBoxStream << "\n>>Experiment started at " << pt::to_simple_string(TIMESTAMP) << "\n";
      _log->PrintExperiment();
   }
}

void SSv3ControlPanel::on_stopExperimentButton_clicked()
{
   CheckErrors(_device->StopExperiment());
   if (_errThrown)
   {
      _errThrown = false;
      return;
   }
   ui.stepAddButton->setEnabled(true);
   ui.startExperimentButton->setEnabled(true);
   ui.experimentList->blockSignals(false);
   ui.stopExperimentButton->setEnabled(false);
   if (ui.experimentList->selectedItems().count() > 0)
      on_experimentList_selectionChanged();
   if (ui.laserOnOffButton->isChecked())
      LaserOnOff(true);
   else if (ui.fireOnOffButton->isChecked())
      FireOnOff(true);
   ui.laserPropertiesGroup->setEnabled(true);
   ui.laserProfileGroup->setEnabled(true);
   ui.sequenceGroup->setEnabled(true);
   ui.scanGroup->setEnabled(true);
   if (ui.fireOnOffButton->isChecked() == false)
   {
      CheckErrors(_device->Fire(false));
      _errThrown = false;
   }
}

void SSv3ControlPanel::LaserOnOff(bool state)
{
   if (!state)
   {
      ui.laserOnOffButton->setText("Off");
      ui.laserOnOffButton->setChecked(false);
      CheckErrors(_device->AOTFBlank(false));
      _errThrown = false;
   }
   else
   {
      ui.laserOnOffButton->setText("On");
      ui.laserOnOffButton->setChecked(true);
      CheckErrors(_device->AOTFBlank(true));
      _errThrown = false;
   }
}

void SSv3ControlPanel::FireOnOff(bool state)
{
   if (!state)
   {
      ui.fireOnOffButton->setText("Fire Off");
      ui.fireOnOffButton->setChecked(false);
      CheckErrors(_device->Fire(false));
      _errThrown = false;
   }
   else
   {
      ui.fireOnOffButton->setText("Fire On");
      ui.fireOnOffButton->setChecked(true);
      CheckErrors(_device->Fire(true));
      _errThrown = false;
   }
}

void SSv3ControlPanel::ShutterOnOff(bool state)
{
   if (!state)
   {
      ui.shutterOnOffButton->setText("Closed");
      ui.shutterOnOffButton->setChecked(false);
      CheckErrors(_device->Shutter(false));
      _errThrown = false;
   }
   else
   {
      ui.shutterOnOffButton->setText("Open");
      ui.shutterOnOffButton->setChecked(true);
      CheckErrors(_device->Shutter(true));
      _errThrown = false;
   }

}

void SSv3ControlPanel::on_laserOnOffButton_clicked()
{
   if (ui.fireOnOffButton->isChecked())
      FireOnOff(false);
   if (!ui.laserOnOffButton->isChecked())
      LaserOnOff(false);
   else
   {
      if (!ui.shutterOnOffButton->isChecked())
         ui.shutterOnOffButton->setChecked(true);
      LaserOnOff(true);
   }
}

void SSv3ControlPanel::on_fireOnOffButton_clicked()
{
   if (ui.laserOnOffButton->isChecked())
      LaserOnOff(false);
   if (!ui.fireOnOffButton->isChecked())
      FireOnOff(false);
   else
   {
      if (!ui.shutterOnOffButton->isChecked())
         ShutterOnOff(true);
      FireOnOff(true);
   }
}

void SSv3ControlPanel::on_shutterOnOffButton_clicked()
{
   LaserOnOff(false);
   FireOnOff(false);
   if (ui.shutterOnOffButton->isChecked())
      ShutterOnOff(true);
   else
      ShutterOnOff(false);
}

void SSv3ControlPanel::on_swTriggerButton_clicked()
{
   //Convert ms to timer reset value (16 bit timer)
   unsigned short duration = ui.triggerLengthSpinBox->value() * 2048 - 1;
   CheckErrors(_device->SendSWTrigger(duration));
   _errThrown = false;
}

void SSv3ControlPanel::SetOneLaser(int line, int value)
{
   unsigned short sending = std::round((double)value * 1023.0 / 100.0);
   CheckErrors(_device->SingleLaserPower((unsigned char)line, sending));
   if (_errThrown)
   {
      _errThrown = false;
      return;
   }
   _currentProfile[line] = sending;
}

void SSv3ControlPanel::on_useCurrentLasersButton_clicked()
{
   std::vector<int> temp{ _currentProfile };
   ui.laser0spinbox->setValue(std::round((double)_previousProfile[0] / 1023.0 * 100.0));
   ui.laser1spinbox->setValue(std::round((double)_previousProfile[1] / 1023.0 * 100.0));
   ui.laser2spinbox->setValue(std::round((double)_previousProfile[2] / 1023.0 * 100.0));
   ui.laser3spinbox->setValue(std::round((double)_previousProfile[3] / 1023.0 * 100.0));
   ui.laser4spinbox->setValue(std::round((double)_previousProfile[4] / 1023.0 * 100.0));
   ui.laser5spinbox->setValue(std::round((double)_previousProfile[5] / 1023.0 * 100.0));
   ui.laser6spinbox->setValue(std::round((double)_previousProfile[6] / 1023.0 * 100.0));
   _previousProfile = temp;
}

void SSv3ControlPanel::on_zeroLasersButton_clicked()
{
   if (_currentProfile == std::vector<int>(8, 0))
      return;
   _previousProfile = _currentProfile;
   for (int i = 0; i < 8; i++)
   {
      _currentProfile[i] = 0;
   }
   ui.laser0spinbox->setValue(_currentProfile[0]);
   ui.laser1spinbox->setValue(_currentProfile[1]);
   ui.laser2spinbox->setValue(_currentProfile[2]);
   ui.laser3spinbox->setValue(_currentProfile[3]);
   ui.laser4spinbox->setValue(_currentProfile[4]);
   ui.laser5spinbox->setValue(_currentProfile[5]);
   ui.laser6spinbox->setValue(_currentProfile[6]);
}

void SSv3ControlPanel::on_newProfileButton_clicked()
{
   if (_profiles.size() == 32)
      return;
   ProfileAddDlg dialog(this);
   if (dialog.exec())
   {
      std::vector<int> newProf(8, 0);
      if (dialog.radio405->isChecked())
         newProf[0] = std::round((double)dialog.spin405->value() * 1023.0 / 100.0);
      if (dialog.radio445->isChecked())
         newProf[1] = std::round((double)dialog.spin445->value() * 1023.0 / 100.0);
      if (dialog.radio488->isChecked())
         newProf[2] = std::round((double)dialog.spin488->value() * 1023.0 / 100.0);
      if (dialog.radio515->isChecked())
         newProf[3] = std::round((double)dialog.spin515->value() * 1023.0 / 100.0);
      if (dialog.radio535->isChecked())
         newProf[4] = std::round((double)dialog.spin535->value() * 1023.0 / 100.0);
      if (dialog.radio560->isChecked())
         newProf[5] = std::round((double)dialog.spin560->value() * 1023.0 / 100.0);
      if (dialog.radio642->isChecked())
         newProf[6] = std::round((double)dialog.spin642->value() * 1023.0 / 100.0);

      int number = _profiles.size();
      UpdateProfile(number, newProf);
      if (_log != nullptr)
      {
         _log->_textBoxStream << "\n>>Excitation profile added:\n";
         _log->PrintProfile(number);
      }
   }
}

void SSv3ControlPanel::on_addProfileButton_clicked()
{
   if (_profiles.size() == 32)
      return;
   int number = _profiles.size();
   UpdateProfile(number, _currentProfile);
   if (_log != nullptr)
   {
      _log->_textBoxStream << "\n>>Excitation profile added:\n";
      _log->PrintProfile(number);
   }
}

void SSv3ControlPanel::on_updateProfileButton_clicked()
{
   if (ui.laserProfileTable->selectedItems().count() == 0)
      return;
   int number = ui.laserProfileTable->currentRow();
   UpdateProfile(number, _currentProfile);
   if (_log != nullptr)
   {
      _log->_textBoxStream << "\n>>Excitation profile changed:\n";
      _log->PrintProfile(number);
   }
}

void SSv3ControlPanel::on_setProfileButton_clicked()
{
   if (ui.laserProfileTable->selectedItems().count() == 0)
      return;
   int number = ui.laserProfileTable->currentRow();
   if (_profiles[number] == _currentProfile)
      return;
   CheckErrors(_device->LoadExcitationProfile(number));
   if (_errThrown)
   {
      _errThrown = false;
      return;
   }
   _previousProfile = _currentProfile;
   _currentProfile = _profiles[number];
   ui.laser0spinbox->setValue(std::round((double)_currentProfile[0] / 1023.0 * 100.0));
   ui.laser1spinbox->setValue(std::round((double)_currentProfile[1] / 1023.0 * 100.0));
   ui.laser2spinbox->setValue(std::round((double)_currentProfile[2] / 1023.0 * 100.0));
   ui.laser3spinbox->setValue(std::round((double)_currentProfile[3] / 1023.0 * 100.0));
   ui.laser4spinbox->setValue(std::round((double)_currentProfile[4] / 1023.0 * 100.0));
   ui.laser5spinbox->setValue(std::round((double)_currentProfile[5] / 1023.0 * 100.0));
   ui.laser6spinbox->setValue(std::round((double)_currentProfile[6] / 1023.0 * 100.0));
}

void SSv3ControlPanel::UpdateProfile(int profNumber, std::vector<int> prof)
{
   //The scan card API needs unsigned short[8]
   unsigned short sending[8];
   for (int i = 0; i < 8; i++)
      sending[i] = prof[i];
   CheckErrors(_device->MakeExcitationProfile(profNumber, sending));
   //Only add the profile to the table and list if API call succeeds
   if (_errThrown)
   {
      _errThrown = false;
      return;
   }
   QTableWidget *tbl = ui.laserProfileTable;
   if (profNumber == tbl->rowCount())
   {
      tbl->insertRow(profNumber);
      for (int i = 0; i < 8; i++)
      {
         QTableWidgetItem *item{ new QTableWidgetItem };
         item->setData(Qt::EditRole, std::round((double)prof[i] / 1023.0 *  100.0));
         tbl->setItem(profNumber, i, item);
         int b = i;
      }
      _profiles.push_back(prof);
   }
   else
   {
      for (int i = 0; i < 8; i++)
         tbl->item(profNumber, i)->setData(Qt::EditRole, std::round((double)prof[i] / 1023.0 * 100.0));
      _profiles[profNumber] = prof;
   }
}

void SSv3ControlPanel::on_newSequenceButton_clicked()
{
   AddCustomDialog dialog(this);
   if (ui.sequenceTable->rowCount() == 32)
      return;
   if (dialog.exec())
   {
      double first, last, range, step;
      int number;
      std::vector<int> seq;
      std::string type;
      if (dialog.linearCustomTab->currentIndex() == 0)
      {
         type = "Linear";
         number = dialog.numberAnglesSpin->value();
         last = dialog.lastAngleSpin->value();
         first = dialog.firstAngleSpin->value();
         range = last - first;
         step = range / ((double)number - 1.0);
         if (dialog.degreesButton->isChecked())
            if (number == 1)
            {
               int val = DegToDac(first);
               seq.push_back(val);
            }
            else
               for (int i = 0; i < number; i++)
               {
                  int val = DegToDac(first + (double)i * step);
                  seq.push_back(val);
               }
         else
            if (number == 1)
               seq.push_back((int)first);
            else
               for (int i = 0; i < number; i++)
                  seq.push_back((int)(first + (double)i * step));
      }
      else
      {
         type = "Custom";
         number = dialog.customTable->rowCount();
         if (number < 1)
            return;
         if (dialog.degreesButton->isChecked())
            for (int i = 0; i < number; i++)
               seq.push_back(DegToDac(dialog.customTable->item(i, 0)->text().toInt()));
         else
            for (int i = 0; i < number; i++)
               seq.push_back(dialog.customTable->item(i, 0)->text().toInt());
      }
      unsigned short *sending = new unsigned short[seq.size()];
      if (sending == nullptr)
         return;
      for (int i = 0; i < seq.size(); i++)
         sending[i] = (unsigned short)seq[i];
      const unsigned char seqnum = _sequences.size();
      const unsigned short seqlen = seq.size();
      CheckErrors(_device->LoadAngles(seqnum, seqlen, sending));
      delete[] sending;
      if (_errThrown)
      {
         _errThrown = false;
         return;
      }
      _sequences.push_back(seq);

      int row = ui.sequenceTable->rowCount();
      ui.sequenceTable->insertRow(row);
      QTableWidgetItem *item0, *item1, *item2, *item3;
      first = seq[0];
      last = *(seq.end() - 1);
      if (ui.degreesButton->isChecked())
      {
         first = DacToDeg(first);
         last = DacToDeg(last);
      }
      item0 = new QTableWidgetItem();
      item0->setText(QString::fromStdString(type));
      item1 = new QTableWidgetItem();
      item1->setText(QString::number(first));
      item2 = new QTableWidgetItem();
      item2->setText(QString::number(last));
      item3 = new QTableWidgetItem();
      item3->setText(QString::number(seq.size()));
      ui.sequenceTable->setItem(row, 0, item0);
      ui.sequenceTable->setItem(row, 1, item1);
      ui.sequenceTable->setItem(row, 2, item2);
      ui.sequenceTable->setItem(row, 3, item3);
      if (_log != nullptr)
      {
         _log->_textBoxStream << "\n>>Angle sequence added:\n";
         _log->PrintSequence(_sequences.size() - 1);
      }
   }
}

int SSv3ControlPanel::DegToDac(double val)
{
   double dac;
   if (_calibrationFactor[0] != 0)
   {
      dac = sqrt(4 * _calibrationFactor[0] * (val - _calibrationFactor[2]) + _calibrationFactor[1] * _calibrationFactor[1]);
      dac -= _calibrationFactor[1];
      dac /= 2 * _calibrationFactor[0];
   }
   else
      dac = (val - _calibrationFactor[2]) / _calibrationFactor[1];
   //quick check that dac is valid
   dac = dac < 0 ? 0 : dac;
   return (unsigned short)dac;
}

double SSv3ControlPanel::DacToDeg(int val)
{
   double dac = (double)val;
   double deg = _calibrationFactor[0] * dac * dac + _calibrationFactor[1] * dac + _calibrationFactor[2];
   deg = std::round(deg * 100.0) / 100.0;
   return deg;
}

void SSv3ControlPanel::on_degreesButton_clicked()
{
   _dontUpdate = true;
   double lim = DacToDeg(0x8000);
   double step = 0.25;
   double x = DacToDeg(ui.xParkSlider->value());
   double y = DacToDeg(ui.yParkSlider->value());
   double r = DacToDeg(ui.radiusSlider->value());
   //Update the scan controls to degree values
   ui.radiusSpin->setDecimals(2);
   ui.radiusSpin->setRange(0.0, lim);
   ui.radiusSpin->setSingleStep(step);
   ui.radiusSpin->setValue(r);
   ui.xParkSpin->setDecimals(2);
   ui.yParkSpin->setDecimals(2);
   ui.xParkSpin->setRange(-lim, lim);
   ui.yParkSpin->setRange(-lim, lim);
   ui.xParkSpin->setSingleStep(step);
   ui.yParkSpin->setSingleStep(step);
   ui.xParkSpin->setValue(x);
   ui.yParkSpin->setValue(y);
   _dontUpdate = false;

   //Update the sequence list table
   ui.degreesButton->blockSignals(true);
   ui.dacButton->blockSignals(false);
   int count = ui.sequenceTable->rowCount();
   if (count < 1)
      return;
   QTableWidgetItem *item;
   int value;
   for (int i = 0; i < count; i++)
   {
      item = ui.sequenceTable->item(i, 1);
      value = *_sequences[i].begin();
      item->setText(QString::number(DacToDeg(value)));
      item = ui.sequenceTable->item(i, 2);
      value = *(_sequences[i].end() - 1);
      item->setText(QString::number(DacToDeg(value)));
   }
}

void SSv3ControlPanel::on_dacButton_clicked()
{
   _dontUpdate = true;
   double lim = 0x8000;
   double step = 0x80;
   //Change the scan controls to DAC units
   double x = ui.xParkSlider->value();
   double y = ui.yParkSlider->value();
   double r = ui.radiusSlider->value();
   ui.radiusSpin->setDecimals(0);
   ui.radiusSpin->setRange(0, lim);
   ui.radiusSpin->setSingleStep(step);
   ui.radiusSpin->setValue(r);
   ui.xParkSpin->setDecimals(0);
   ui.yParkSpin->setDecimals(0);
   ui.xParkSpin->setRange(-lim, lim);
   ui.yParkSpin->setRange(-lim, lim);
   ui.xParkSpin->setSingleStep(step);
   ui.yParkSpin->setSingleStep(step);
   ui.xParkSpin->setValue(x);
   ui.yParkSpin->setValue(y);
   _dontUpdate = false;

   //Update the sequence list table
   ui.dacButton->blockSignals(true);
   ui.degreesButton->blockSignals(false);
   if (!ui.dacButton->isChecked())
      return;
   int count = ui.sequenceTable->rowCount();
   if (count < 1)
      return;
   QTableWidgetItem *item;
   double value;
   for (int i = 0; i < count; i++)
   {
      item = ui.sequenceTable->item(i, 1);
      item->setText(QString::number(*_sequences[i].begin()));
      item = ui.sequenceTable->item(i, 2);
      item->setText(QString::number(*(_sequences[i].end() - 1)));
   }
}

void SSv3ControlPanel::on_modifySequenceButton_clicked()
{
   if (ui.sequenceTable->currentRow() < 0)
      return;
   AddCustomDialog dialog(this);
   if (dialog.exec())
   {
      double first, last, range, step;
      int number;
      std::vector<int> seq;
      std::string type;
      if (dialog.linearCustomTab->currentIndex() == 0)
      {
         type = "Linear";
         number = dialog.numberAnglesSpin->value();
         last = dialog.lastAngleSpin->value();
         first = dialog.firstAngleSpin->value();
         range = last - first;
         step = range / ((double)number - 1.0);
         if (dialog.degreesButton->isChecked())
            if (number == 1)
            {
               int val = DegToDac(first);
               seq.push_back(val);
            }
            else
               for (int i = 0; i < number; i++)
               {
                  int val = DegToDac(first + (double)i * step);
                  seq.push_back(val);
               }
         else
            if (number == 1)
               seq.push_back((int)first);
            else
               for (int i = 0; i < number; i++)
                  seq.push_back((int)(first + (double)i * step));
      }
      else
      {
         type = "Custom";
         number = dialog.customTable->rowCount();
         if (number < 1)
            return;
         if (dialog.degreesButton->isChecked())
            for (int i = 0; i < number; i++)
               seq.push_back(DegToDac(dialog.customTable->item(i, 0)->text().toInt()));
         else
            for (int i = 0; i < number; i++)
               seq.push_back(dialog.customTable->item(i, 0)->text().toInt());
      }

      int row = ui.sequenceTable->currentRow();
      unsigned short *sending = new unsigned short[seq.size()];
      for (int i = 0; i < seq.size(); i++)
         sending[i] = (unsigned short)seq[i];
      const unsigned char seqnum = row;
      const unsigned short seqlen = seq.size();
      CheckErrors(_device->LoadAngles(seqnum, seqlen, sending));
      if (_errThrown)
      {
         _errThrown = false;
         return;
      }
      _sequences[seqnum] = seq;

      QTableWidgetItem *item;
      first = seq[0];
      last = *(seq.end() - 1);
      if (ui.degreesButton->isChecked())
      {
         first = DacToDeg(first);
         last = DacToDeg(last);
      }
      item = ui.sequenceTable->item(row, 0);
      item->setText(QString::fromStdString(type));
      item = ui.sequenceTable->item(row, 1);
      item->setText(QString::number(first));
      item = ui.sequenceTable->item(row, 2);
      item->setText(QString::number(last));
      item = ui.sequenceTable->item(row, 3);
      item->setText(QString::number(seq.size()));
      if (_log != nullptr)
      {
         _log->_textBoxStream << "\n>>Angle sequence modified:\n";
         _log->PrintSequence(seqnum);
      }
   }
}

void SSv3ControlPanel::on_testSequenceButton_clicked()
{
   if (ui.testSequenceButton->isChecked())
   {
      if (ui.sequenceTable->currentRow() < 0 || ui.laserProfileTable->currentRow() < 0)
      {
         ErrorDialog dialog(this, "You must have 1 sequence and 1 laser profile selected to begin");
         if (dialog.exec());
         ui.testSequenceButton->setChecked(false);
         return;
      }
      int sequence = ui.sequenceTable->currentRow();
      int profile = ui.laserProfileTable->currentRow();
      CheckErrors(_device->ClearExperiment());
      if (_errThrown)
      {
         _errThrown = false;
         ui.testSequenceButton->setChecked(false);
         return;
      }
      CheckErrors(_device->AddExperimentStep((unsigned char)sequence, (unsigned char)profile));
      if (_errThrown)
      {
         _errThrown = false;
         ui.testSequenceButton->setChecked(false);
         return;
      }
      CheckErrors(_device->Loop(true, 0));
      if (_errThrown)
      {
         _errThrown = false;
         ui.testSequenceButton->setChecked(false);
         return;
      }
      CheckErrors(_device->StartExperiment());
      if (_errThrown)
      {
         _errThrown = false;
         ui.testSequenceButton->setChecked(false);
         return;
      }
      ui.experimentGroup->setEnabled(false);
      ui.laserPropertiesGroup->setEnabled(false);
      ui.laserProfileGroup->setEnabled(false);
      ui.sequenceTable->setEnabled(false);
      ui.newSequenceButton->setEnabled(false);
      ui.modifySequenceButton->setEnabled(false);
      ui.setCalibrationButton->setEnabled(false);
      ui.scanGroup->setEnabled(false);
   }
   else
   {
      CheckErrors(_device->StopExperiment());
      if (_errThrown)
      {
         _errThrown = false;
         ui.testSequenceButton->setChecked(true);
         return;
      }
      if (ui.fireOnOffButton->isChecked() == false)
      {
         CheckErrors(_device->Fire(false));
         _errThrown = false;
      }
      ui.experimentGroup->setEnabled(true);
      ui.laserPropertiesGroup->setEnabled(true);
      ui.laserProfileGroup->setEnabled(true);
      ui.sequenceTable->setEnabled(true);
      ui.newSequenceButton->setEnabled(true);
      ui.modifySequenceButton->setEnabled(true);
      ui.setCalibrationButton->setEnabled(true);
      ui.scanGroup->setEnabled(true);
   }
}

void SSv3ControlPanel::on_resetButton_clicked()
{
   ResetDialog dialog(this);
   if (dialog.exec())
   {
      _device->Reset();
      _device.reset();
      ui.experimentList->clear();
      ui.sequenceTable->setRowCount(0);
      ui.laserProfileTable->setRowCount(0);
      ui.controllerTable->setRowCount(0);
      _sequences.clear();
      _profiles.clear();
      _experiment.clear();
      _devNumber = 0;
      _loopOnOff = false;
      _loopTo = 0; ui.controllerStatusLabel->setText(QString("Disconnected"));
      ui.controllerStatusLabel->setStyleSheet("QLabel {background-color : white; color : red; }");
      ui.controllerErrorLabel->setText(QString(""));
      ui.controllerErrorLabel->setStyleSheet("QLabel {background-color : white; color : green; }"); ui.disconnectControllerButton->setEnabled(false);
      ui.visorButton->setEnabled(false);
      ui.connectControllerButton->setEnabled(true);
      ui.laserPropertiesGroup->setEnabled(false);
      ui.experimentGroup->setEnabled(false);
      ui.scanGroup->setEnabled(false);
      ui.laserProfileGroup->setEnabled(false);
      ui.sequenceGroup->setEnabled(false);
      ui.usbInterfaceButton->setEnabled(false);
      if (_log != nullptr)
         _log->_textBoxStream << "\n>>Controller reset\n";
   }
}

void SSv3ControlPanel::on_usbInterfaceButton_clicked()
{
   HIDInterfaceDialog dialog(this);
   if (dialog.exec())
   {
      HIDTerminal *terminal = new HIDTerminal(this, _device, _demoMode);
      terminal->setAttribute(Qt::WA_DeleteOnClose);
      terminal->show();
   }
}

void SSv3ControlPanel::on_radiusSlider_sliderMoved()
{
   ui.radiusSpin->blockSignals(true);
   double value = ui.radiusSlider->value();
   if (ui.degreesButton->isChecked())
      value = DacToDeg(value);
   ui.radiusSpin->setValue(value);
   if (ui.scanButton->isChecked() && !_dontUpdate)
   {
      CheckErrors(_device->ScanRadius(ui.radiusSlider->value()));
      _errThrown = false;
   }
   ui.radiusSpin->blockSignals(false);
}

void SSv3ControlPanel::on_xParkSlider_sliderMoved()
{
   ui.xParkSpin->blockSignals(true);
   double value = ui.xParkSlider->value();
   if (ui.degreesButton->isChecked() && value > 0)
   {
      ui.xParkSpin->setValue(DacToDeg(value));
      _parkLocation[0] = DacToDeg(value);
   }
   else if (ui.degreesButton->isChecked() && value < 0)
   {
      ui.xParkSpin->setValue(-DacToDeg(-value));
      _parkLocation[0] = -DacToDeg(-value);
   }
   else if (ui.dacButton->isChecked() && value > 0)
   {
      ui.xParkSpin->setValue(value);
      _parkLocation[0] = DacToDeg(value);
   }
   else
   {
      ui.xParkSpin->setValue(value);
      _parkLocation[0] = -DacToDeg(-value);
   }
   if (ui.locationParkButton->isChecked() && !_dontUpdate)
      SetLocationPark();
   ui.xParkSpin->blockSignals(false);
}

void SSv3ControlPanel::on_yParkSlider_sliderMoved()
{
   ui.yParkSpin->blockSignals(true);
   double value = ui.yParkSlider->value();
   if (ui.degreesButton->isChecked() && value > 0)
   {
      ui.yParkSpin->setValue(DacToDeg(value));
      _parkLocation[1] = DacToDeg(value);
   }
   else if (ui.degreesButton->isChecked() && value < 0)
   {
      ui.yParkSpin->setValue(-DacToDeg(-value));
      _parkLocation[1] = -DacToDeg(-value);
   }
   else if (ui.dacButton->isChecked() && value > 0)
   {
      ui.yParkSpin->setValue(value);
      _parkLocation[1] = DacToDeg(value);
   }
   else
   {
      ui.yParkSpin->setValue(value);
      _parkLocation[1] = -DacToDeg(-value);
   }
   if (ui.locationParkButton->isChecked() && !_dontUpdate)
      SetLocationPark();
   ui.yParkSpin->blockSignals(true);
}

void SSv3ControlPanel::on_radiusSpin_valueChanged()
{
   ui.radiusSlider->blockSignals(true);
   double value = ui.radiusSpin->value();
   if (ui.degreesButton->isChecked())
      value = DegToDac(value);
   ui.radiusSlider->setValue(value);
   if (ui.scanButton->isChecked() && !_dontUpdate)
   {
      CheckErrors(_device->ScanRadius(value));
      _errThrown = false;
   }
   ui.radiusSlider->blockSignals(false);
}

void SSv3ControlPanel::on_xParkSpin_valueChanged()
{
   ui.xParkSlider->blockSignals(true);
   double value = ui.xParkSpin->value();
   if (ui.degreesButton->isChecked() && value > 0)
   {
      _parkLocation[0] = value;
      ui.xParkSlider->setValue(DegToDac(value));
   }
   else if (ui.degreesButton->isChecked() && value < 0)
   {
      _parkLocation[0] = value;
      ui.xParkSlider->setValue(-DegToDac(-value));
   }
   else if (ui.dacButton->isChecked() && value > 0)
   {
      _parkLocation[0] = DacToDeg(value);
      ui.xParkSlider->setValue(value);
   }
   else
   {
      _parkLocation[0] = -DacToDeg(-value);
      ui.xParkSlider->setValue(value);
   }
   if (ui.locationParkButton->isChecked() && !_dontUpdate)
      SetLocationPark();
   ui.xParkSlider->blockSignals(false);
}

void SSv3ControlPanel::on_yParkSpin_valueChanged()
{
   ui.yParkSlider->blockSignals(true);
   double value = ui.yParkSpin->value();
   if (ui.degreesButton->isChecked() && value > 0)
   {
      _parkLocation[1] = value;
      ui.yParkSlider->setValue(DegToDac(value));
   }
   else if (ui.degreesButton->isChecked() && value < 0)
   {
      _parkLocation[1] = value;
      ui.yParkSlider->setValue(-DegToDac(-value));
   }
   else if (ui.dacButton->isChecked() && value > 0)
   {
      _parkLocation[1] = DacToDeg(value);
      ui.yParkSlider->setValue(value);
   }
   else
   {
      _parkLocation[1] = -DacToDeg(-value);
      ui.yParkSlider->setValue(value);
   }
   if (ui.locationParkButton->isChecked() && !_dontUpdate)
      SetLocationPark();
   ui.yParkSlider->blockSignals(false);
}

void SSv3ControlPanel::SetLocationPark()
{
   unsigned short sending[2];
   if (_parkLocation[0] < 0)
      sending[0] = 0x7fff - DegToDac(0 - _parkLocation[0]);
   else
      sending[0] = 0x7fff + DegToDac(_parkLocation[0]);
   if (_parkLocation[1] < 0)
      sending[1] = 0x7fff - DegToDac(0 - _parkLocation[1]);
   else
      sending[1] = 0x7fff + DegToDac(_parkLocation[1]);
   CheckErrors(_device->LocationPark(sending));
   _errThrown = false;
}

void SSv3ControlPanel::on_scanButton_clicked()
{
   if (ui.scanButton->isChecked())
   {
      unsigned short valDac = ui.radiusSlider->value();
      CheckErrors(_device->ScanRadius(valDac));
      if (_errThrown)
      {
         _errThrown = false;
         ui.scanButton->setChecked(false);
         return;
      }
      ui.centerParkButton->setChecked(false);
      ui.locationParkButton->setChecked(false);
      ui.tirButton->setChecked(false);
      return;
   }
   CheckErrors(_device->CenterPark());
   if (_errThrown)
   {
      _errThrown = false;
      ui.scanButton->setChecked(true);
      return;
   }
   ui.centerParkButton->setChecked(true);
}

void SSv3ControlPanel::on_centerParkButton_clicked()
{
   CheckErrors(_device->CenterPark());
   {
      _errThrown = false;
      ui.centerParkButton->setChecked(false);
      return;
   }
   ui.scanButton->setChecked(false);
   ui.centerParkButton->setChecked(true);
   ui.locationParkButton->setChecked(false);
   ui.tirButton->setChecked(false);
}

void SSv3ControlPanel::on_locationParkButton_clicked()
{
   if (ui.locationParkButton->isChecked())
   {
      unsigned short sending[2];
      if (_parkLocation[0] < 0)
         sending[0] = 0x7fff - DegToDac(0 - _parkLocation[0]);
      else
         sending[0] = 0x7fff + DegToDac(_parkLocation[0]);
      if (_parkLocation[1] < 0)
         sending[1] = 0x7fff - DegToDac(0 - _parkLocation[1]);
      else
         sending[1] = 0x7fff + DegToDac(_parkLocation[1]);
      CheckErrors(_device->LocationPark(sending));
      if (_errThrown)
      {
         _errThrown = false;
         ui.locationParkButton->setChecked(false);
         return;
      }
      ui.scanButton->setChecked(false);
      ui.tirButton->setChecked(false);
      ui.centerParkButton->setChecked(false);
      return;
   }
   CheckErrors(_device->CenterPark());
   if (_errThrown)
   {
      _errThrown = false;
      ui.locationParkButton->setChecked(true);
      return;
   }
   ui.centerParkButton->setChecked(true);
}

void SSv3ControlPanel::on_tirButton_clicked()
{
   if (ui.tirButton->isChecked())
   {
      CheckErrors(_device->TIRF());
      if (_errThrown)
      {
         _errThrown = false;
         ui.tirButton->setChecked(false);
         return;
      }
      ui.scanButton->setChecked(false);
      ui.centerParkButton->setChecked(false);
      ui.locationParkButton->setChecked(false);
      return;
   }
   CheckErrors(_device->CenterPark());
   if (_errThrown)
   {
      _errThrown = false;
      ui.tirButton->setChecked(true);
      return;
   }
   ui.centerParkButton->setChecked(true);
}

void SSv3ControlPanel::on_tirSlider_sliderMoved()
{
   if (ui.locationParkButton->isChecked())
   {
      ui.tirButton->setChecked(true);
      on_tirButton_clicked();
   }
   ui.tirSpin->blockSignals(true);
   ui.tirSpin->setValue(ui.tirSlider->value());
   ui.tirSpin->blockSignals(false);
   if (!_dontUpdate)
   {
      CheckErrors(_device->TIRF((unsigned short)ui.tirSlider->value()));
      if (_errThrown)
      {
         _errThrown = false;
         return;
      }
   }
   if (ui.tirButton->isChecked() == false)
   {
      ui.tirButton->setChecked(true);
      ui.scanButton->setChecked(false);
      ui.centerParkButton->setChecked(false);
      ui.locationParkButton->setChecked(false);
   }
   _tirRadius = ui.tirSlider->value();
}

void SSv3ControlPanel::on_xCenterSlider_sliderMoved()
{
   ui.xCenterSpin->blockSignals(true);
   ui.xCenterSpin->setValue(ui.xCenterSlider->value());
   ui.xCenterSpin->blockSignals(false);
   CheckErrors(_device->ScanCenter(true, (unsigned short)ui.xCenterSpin->value()));
   _xCenter = ui.xCenterSlider->value();
   _errThrown = false;
}

void SSv3ControlPanel::on_yCenterSlider_sliderMoved()
{
   ui.yCenterSpin->blockSignals(true);
   ui.yCenterSpin->setValue(ui.yCenterSlider->value());
   ui.yCenterSpin->blockSignals(false);
   CheckErrors(_device->ScanCenter(false, (unsigned short)ui.yCenterSpin->value()));
   _yCenter = ui.yCenterSlider->value();
   _errThrown = false;
}

void SSv3ControlPanel::on_phaseSlider_sliderMoved()
{
   ui.phaseSpin->blockSignals(true);
   ui.phaseSpin->setValue(ui.phaseSlider->value());
   ui.phaseSpin->blockSignals(false);
   CheckErrors(_device->AdjustPhase((unsigned short)ui.phaseSpin->value()));
   _phase = ui.phaseSlider->value();
   _errThrown = false;
}

void SSv3ControlPanel::on_yScaleSlider_sliderMoved()
{
   ui.scanButton->setChecked(true);
   on_scanButton_clicked();
   ui.yScaleSpin->blockSignals(true);
   ui.yScaleSpin->setValue(ui.yScaleSlider->value());
   ui.yScaleSpin->blockSignals(false);
   CheckErrors(_device->YAmpCorrection((unsigned short)ui.yScaleSpin->value()));
   _yScale = ui.yScaleSlider->value();
   _errThrown = false;
}

void SSv3ControlPanel::on_tirSpin_valueChanged()
{
   if (ui.locationParkButton->isChecked())
   {
      ui.tirButton->setChecked(true);
      on_tirButton_clicked();
   }
   ui.tirSlider->blockSignals(true);
   ui.tirSlider->setValue(ui.tirSpin->value());
   ui.tirSlider->blockSignals(false);
   if (!_dontUpdate)
   {
      CheckErrors(_device->TIRF((unsigned short)ui.tirSpin->value()));
      if (_errThrown)
      {
         _errThrown = false;
         return;
      }
   }
   if (ui.tirButton->isChecked() == false)
   {
      ui.scanButton->setChecked(false);
      ui.centerParkButton->setChecked(false);
      ui.locationParkButton->setChecked(false);
      ui.tirButton->setChecked(true);
   }
   _tirRadius = ui.tirSpin->value();
}

void SSv3ControlPanel::on_xCenterSpin_valueChanged()
{
   ui.xCenterSlider->blockSignals(true);
   ui.xCenterSlider->setValue(ui.xCenterSpin->value());
   ui.xCenterSlider->blockSignals(false);
   CheckErrors(_device->ScanCenter(true, (unsigned short)ui.xCenterSpin->value()));
   _errThrown = false;
   _xCenter = ui.xCenterSpin->value();
}

void SSv3ControlPanel::on_yCenterSpin_valueChanged()
{
   ui.yCenterSlider->blockSignals(true);
   ui.yCenterSlider->setValue(ui.yCenterSpin->value());
   ui.yCenterSlider->blockSignals(false);
   CheckErrors(_device->ScanCenter(false, (unsigned short)ui.yCenterSpin->value()));
   _errThrown = false;
   _yCenter = ui.yCenterSpin->value();
}

void SSv3ControlPanel::on_phaseSpin_valueChanged()
{
   ui.phaseSlider->blockSignals(true);
   ui.phaseSlider->setValue(ui.phaseSpin->value());
   ui.phaseSlider->blockSignals(false);
   CheckErrors(_device->AdjustPhase((unsigned short)ui.phaseSpin->value()));
   _errThrown = false;
   _phase = ui.phaseSpin->value();
}

void SSv3ControlPanel::on_yScaleSpin_valueChanged()
{
   ui.scanButton->setChecked(true);
   on_scanButton_clicked();
   ui.yScaleSlider->blockSignals(true);
   ui.yScaleSlider->setValue(ui.yScaleSpin->value());
   ui.yScaleSlider->blockSignals(false);
   CheckErrors(_device->YAmpCorrection((unsigned short)ui.yScaleSpin->value()));
   _errThrown = false;
   _yScale = ui.yScaleSpin->value();
}

void SSv3ControlPanel::on_frequencySpin_valueChanged()
{
   int val = ui.frequencySpin->value();
   val *= 0x2e23;
   val /= 1100;
   CheckErrors(_device->AdjustFrequency(val));
   _errThrown = false;
   _frequency = ui.frequencySpin->value();
}

void SSv3ControlPanel::on_setCalibrationButton_clicked()
{
   SetCalibrationValuesDialog dialog(this, _calibrationFactor);
   if (dialog.exec())
   {
      _calibrationFactor[0] = dialog._quadValue->text().toDouble();
      _calibrationFactor[1] = dialog._linValue->text().toDouble();
      _calibrationFactor[2] = dialog._constValue->text().toDouble();
      if (ui.degreesButton->isChecked())
         on_degreesButton_clicked();
   }
   if (_log != nullptr)
   {
      _log->_textBoxStream << "\n>>Deg to DAC conversion constants changed: C1="
         << _calibrationFactor[0] << " C2=" << _calibrationFactor[1]
         << " C3=" << _calibrationFactor[2] << "\n";
   }
}

void SSv3ControlPanel::on_advancedScanGroup_clicked()
{
   if (ui.advancedScanGroup->isChecked())
      ui.calibrateButton->setEnabled(true);
}

void SSv3ControlPanel::on_calibrateButton_clicked()
{
   CalibratorDevice *calibratorDevice = new CalibratorDevice(this);
   calibratorDevice->setAttribute(Qt::WA_DeleteOnClose);
   calibratorDevice->setAttribute(Qt::WA_QuitOnClose, false);
   calibratorDevice->show();
   ui.setCalibrationButton->setEnabled(false);
   connect(calibratorDevice, SIGNAL(destroyed()), this, SLOT(on_calibrator_closed()));
}

void SSv3ControlPanel::on_calibration_sent(double val)
{
   _calibrationFactor[0] = 0;
   _calibrationFactor[1] = val;
   _calibrationFactor[2] = 0;
   if (ui.degreesButton->isChecked())
      on_degreesButton_clicked();
   if (_log != nullptr)
   {
      _log->_textBoxStream << "\n>>Deg to DAC conversion constants changed: C1=" << _calibrationFactor[0] << " C2=" << _calibrationFactor[1] << " C3=" << _calibrationFactor[2] << "\n";
   }
}

void cp::on_calibrator_closed()
{
   ui.setCalibrationButton->setEnabled(true);
}