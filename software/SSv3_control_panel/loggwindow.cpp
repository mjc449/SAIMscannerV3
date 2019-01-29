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

#include "loggwindow.h"
#include <qfiledialog.h>
#include <qstandardpaths.h>
#include <qerrormessage.h>
#include <qmessagebox.h>
#include <boost/date_time.hpp>

namespace pt = boost::posix_time;

#define TIMESTAMP QString::fromStdString(pt::to_simple_string(pt::second_clock::local_time().time_of_day()))

LoggWindow::LoggWindow(SSv3ControlPanel *cp, QWidget *parent)
   : QWidget(parent), _mainWindow(cp)
{
   setupUi(this);
   setAttribute(Qt::WA_NativeWindow);
   setWindowTitle(tr("Experiment logger"));
   SetupWindow();
}

LoggWindow::~LoggWindow()
{
   _mainWindow->startLogging->setEnabled(true);
}

void LoggWindow::closeEvent(QCloseEvent *event)
{
   bool escape{ false };
   do
   {
      QMessageBox::StandardButton confirm = QMessageBox::question(this, tr("Close Log"), tr("Would you like to save the current log before closing? (If autosave is enabled the current location will be used.)"), QMessageBox::Save | QMessageBox::Close | QMessageBox::Cancel, QMessageBox::Cancel);
      if (confirm == QMessageBox::Cancel)
      {
         event->ignore();
         escape = true;
      }
      else if (confirm == QMessageBox::Save)
      {
         if (_autosaveCheckBox->isChecked())
         {
            SaveLog();
            event->accept();
            escape = true;
         }
         else
         {
            QFileDialog fileDialog;
            QString filter = "Text files (*.txt)";
            QString path;
            if (_saveLocation->text().isEmpty())
               path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
            else
               path = _saveLocation->text();
            QString fileName = fileDialog.getSaveFileName(this, tr("Save current log as..."), path, filter, &filter);
            if (!fileName.isEmpty())
            {
               QFileInfo info(fileName);
               if (info.suffix().isEmpty())
                  fileName += tr(".txt");

               QFile oFile(fileName);
               if (!oFile.open(QIODevice::Truncate | QIODevice::Text | QIODevice::WriteOnly))
               {
                  QString msg{ "Could not open the file " };
                  msg += fileName;
                  QErrorMessage err;
                  err.showMessage(msg);
               }
               QTextStream fileStream(&oFile);
               fileStream << _textBox->toPlainText();
               oFile.flush();
               oFile.close();
               event->accept();
               escape = true;
            }
         }
      }
      else
      {
         event->accept();
         escape = true;
      }
   } while (!escape);
}

void LoggWindow::SetupWindow()
{
   this->setWindowFlags(Qt::Window);

   _mainLayout = new QVBoxLayout();
   _textBox = new QTextEdit();
   _textBox->setWordWrapMode(QTextOption::NoWrap);
   _textBox->setReadOnly(true);
   _commentBox = new QTextEdit();
   _commentBox->setWordWrapMode(QTextOption::NoWrap);

   _commentButtonLayout = new QHBoxLayout();
   _addCommentButton = new QPushButton();
   _addCommentButton->setDefault(true);
   _addCommentButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
   _addCommentButton->setText(tr("Add Comment"));
   connect(_addCommentButton, SIGNAL(clicked()), this, SLOT(on_addComment()));
   _clearCommentButton = new QPushButton();
   _clearCommentButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
   _clearCommentButton->setText(tr("Clear Comment"));
   connect(_clearCommentButton, SIGNAL(clicked()), _commentBox, SLOT(&QTextEdit::clear));
   _commentButtonLayout->addWidget(_addCommentButton);
   _commentButtonLayout->addWidget(_clearCommentButton);

   _saveLocation = new QLineEdit();
   _saveLocation->setReadOnly(true);
   _saveLayout = new QHBoxLayout();
   _autosaveCheckBox = new QCheckBox();
   _autosaveCheckBox->setText("Autosave");
   connect(_autosaveCheckBox, SIGNAL(clicked(bool)), SLOT(on_autosaveCheckBox_checked()));
   _saveLocationButton = new QPushButton();
   _saveLocationButton->setFixedSize(20, 20);
   _saveLocationButton->setText("...");
   connect(_saveLocationButton, SIGNAL(clicked()), SLOT(on_saveLocationButton_clicked()));
   _saveLayout->addWidget(_saveLocation);
   _saveLayout->addWidget(_saveLocationButton);
   _saveLayout->addWidget(_autosaveCheckBox);

   _controlButtonLayout = new QHBoxLayout();
   _recordSettingsButton = new QPushButton();
   _recordSettingsButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
   _recordSettingsButton->setText("Record settings");
   connect(_recordSettingsButton, SIGNAL(clicked()), this, SLOT(on_recordSettingsButton_clicked()));
   _recordElementsButton = new QPushButton();
   _recordElementsButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
   _recordElementsButton->setText("Record elements");
   connect(_recordElementsButton, SIGNAL(clicked()), this, SLOT(on_recordElementsButton_clicked()));
   _saveAsButton = new QPushButton();
   _saveAsButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
   _saveAsButton->setText("Save log as");
   connect(_saveAsButton, SIGNAL(clicked()), SLOT(on_saveAsButton_clicked()));
   _closeButton = new QPushButton();
   _closeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
   _closeButton->setText("Close");
   connect(_closeButton, SIGNAL(clicked()), SLOT(close()));
   _controlButtonLayout->addWidget(_saveAsButton);
   _controlButtonLayout->addWidget(_recordElementsButton);
   _controlButtonLayout->addWidget(_recordSettingsButton);
   _controlButtonLayout->addWidget(_closeButton);

   _mainLayout->addWidget(_textBox, 10);
   _mainLayout->addWidget(_commentBox, 2);
   _mainLayout->addLayout(_commentButtonLayout);
   _mainLayout->addLayout(_saveLayout);
   _mainLayout->addLayout(_controlButtonLayout);
   _mainLayout->update();
   setLayout(_mainLayout);

   _textBoxStream = TextEditStream(_textBox);
   std::string date = pt::to_simple_string(pt::second_clock::local_time());
   _textBoxStream
      << "\n***************************************************\n"
      << ">> Log opened " << date
      << "\n***************************************************\n";

   _autosaveTimer = new QTimer(this);
   _autosaveTimer->setInterval(30000);
   _autosaveTimer->setTimerType(Qt::VeryCoarseTimer);
   connect(_autosaveTimer, SIGNAL(timeout()), this, SLOT(SaveLog()));
}

void LoggWindow::on_saveLocationButton_clicked()
{
   QFileDialog dlg;
   QString filter = "Text files (*.txt)";
   QString path;
   if (_saveLocation->text().isEmpty())
      path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
   else
      path = _saveLocation->text();
   QString fileName = QFileDialog::getSaveFileName(this, tr("Save log"), path, filter, &filter);
   if (!fileName.isEmpty())
   {
      QFileInfo info(fileName);
      if (info.suffix().isEmpty())
         fileName += tr(".txt");
      _saveLocation->setText(fileName);
   }
}

void LoggWindow::on_saveAsButton_clicked()
{
   QFileDialog fileDialog;
   QString filter = "Text files (*.txt)";
   QString path;
   if (_saveLocation->text().isEmpty())
      path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
   else
      path = _saveLocation->text();
   QString fileName = fileDialog.getSaveFileName(this, tr("Save current log as..."), path, filter, &filter);
   if (!fileName.isEmpty())
   {
      QFileInfo info(fileName);
      if (info.suffix().isEmpty())
         fileName += tr(".txt");

      QFile oFile(fileName);
      if (!oFile.open(QIODevice::Truncate | QIODevice::Text | QIODevice::WriteOnly))
      {
         std::string msg{ strerror(errno) };
         QMessageBox::critical(this, tr("Error opening config file"), QString::fromStdString(msg));
         return;
      }
      QTextStream fileStream(&oFile);
      fileStream << _textBox->toPlainText();
      oFile.close();
   }
}

void LoggWindow::on_autosaveCheckBox_checked()
{
   if (_autosaveCheckBox->isChecked())
   {
      if (_saveLocation->text().isEmpty())
         on_saveLocationButton_clicked();
      if (_saveLocation->text().isEmpty())
      {
         _autosaveCheckBox->setChecked(false);
         return;
      }
      QString fileName = _saveLocation->text();
      _logFile.setFileName(fileName);
      SaveLog();
      _autosaveTimer->start();
   }
   if (!_autosaveCheckBox->isChecked())
   {
      _autosaveTimer->stop();
      SaveLog();
      _fileStream.setDevice(NULL);
   }
}

void LoggWindow::SaveLog()
{
   if (!_logFile.open(QIODevice::Truncate | QIODevice::Text | QIODevice::WriteOnly))
   {
      QString msg(tr("Could not open the file "));
      msg += _logFile.fileName();
      QErrorMessage err;
      err.showMessage(msg);
   }
   _fileStream.setDevice(&_logFile);
   _fileStream << _textBox->toPlainText();
   _fileStream.flush();
   _logFile.close();
}

void LoggWindow::on_addComment()
{
   _textBoxStream << "--------------------------------------------------------------\n"
      << _commentBox->toPlainText()
      << "\n--------------------------------------------------------------\n";
   _commentBox->clear();
}

void LoggWindow::on_recordSettingsButton_clicked()
{
   _textBox->moveCursor(QTextCursor::End);
   _textBoxStream << "\n>>" << TIMESTAMP
      << "  - Controller settings report for: ";
   if (_mainWindow->_devNumber != -1)
   {
      _textBoxStream 
         << QString::fromStdWString(_mainWindow->_devProduct)
         << " S/N: "
         << QString::fromStdWString(_mainWindow->_devSerialNumber)
         << "\n>>    TIR radius - " << (int)_mainWindow->_tirRadius
         << "\n>>    Scan center (X, Y) - (" << (int)_mainWindow->_xCenter
         << ", " << (int)_mainWindow->_yCenter << ")"
         << "\n>>    Phase - " << (int)_mainWindow->_phase
         << "\n>>    Frequency - " << (double)_mainWindow->_frequency * 1100 / 0x2e23
         << "\n>>    Dac to deg conversion values: C1="
         << _mainWindow->_calibrationFactor[0]
         << " C2=" << _mainWindow->_calibrationFactor[1]
         << " C3=" << _mainWindow->_calibrationFactor[2]
         << "\n";
   }
   else
   {
      _textBoxStream << "No device found\n";
   }
}

void LoggWindow::on_recordElementsButton_clicked()
{
   _textBox->moveCursor(QTextCursor::End);
   _textBoxStream << "\n>>" << TIMESTAMP
      << " - Controller memory report: ";
   if (_mainWindow->_devNumber == -1)
   {
      _textBoxStream << "No device found\n";
      return;
   }
   _textBoxStream << "\n>>     Angle sequences:";
   if (_mainWindow->_sequences.empty())
      _textBoxStream << "\n>>       No sequences loaded";
   else
   {
      _textBoxStream << "\n";
      for (int i = 0; i < _mainWindow->_sequences.size(); i++)
         PrintSequence(i);
   }
   _textBoxStream << "\n>>      Excitation profiles:";
   if (_mainWindow->_profiles.empty())
      _textBoxStream << "\n>>       No profiles loaded";
   else
   {
      _textBoxStream << "\n";
      for (int i = 0; i < _mainWindow->_profiles.size(); i++)
         PrintProfile(i);
   }
   _textBoxStream << "\n";
   PrintExperiment();
   _textBoxStream << "\n";
}

void LoggWindow::PrintSequence(int seqNum)
{
   _textBoxStream << ">>         Sequence " << seqNum + 1 << ")";
   for (auto i = _mainWindow->_sequences[seqNum].begin(); i != _mainWindow->_sequences[seqNum].end(); i++)
      _textBoxStream << " " << _mainWindow->DacToDeg(*i);
   _textBoxStream << "\n";
}

void LoggWindow::PrintProfile(int proNum)
{
   _textBoxStream << ">>         Profile " << proNum + 1 << ")";
   for (auto i = _mainWindow->_profiles[proNum].begin(); i != _mainWindow->_profiles[proNum].end(); i++)
      _textBoxStream << " " << (double)*i * 100.0 / 1023.0;
   _textBoxStream << "\n";
}

void LoggWindow::PrintExperiment()
{
   _textBoxStream << ">>      Experiment setup:";
   if (_mainWindow->_experiment.empty())
      _textBoxStream << " experiment list is empty";
   else
   {
      int number = 1;
      for (auto i = _mainWindow->_experiment.begin(); i != _mainWindow->_experiment.end(); i++)
      {
         _textBoxStream << "\n>>       Step " << number++
            << ") Sequence " << i->sequence
            << ", Profile " << i->profile;
      }
      if (_mainWindow->_loopOnOff)
         _textBoxStream << "\n>>       Looping to step " << (int)(_mainWindow->_loopTo + 1);
      _textBoxStream << "\n";
   }
}