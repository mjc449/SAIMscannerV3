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

#include "setcalibrationvaluesdialog.h"

SetCalibrationValuesDialog::SetCalibrationValuesDialog(QWidget *parent, std::vector<double> current)
    : QDialog(parent)
{
    setupUi(this);

    for (int i = 0; i < 3; i++)
       _oldValues[i] = current[i];

    setWindowTitle("Update calibration");
    _layout = new QGridLayout();
    _quadLabel = new QLabel("Quadratic");
    _linLabel = new QLabel("Linear");
    _constLabel = new QLabel("Constant");
    _quadValue = new QLineEdit;
    _linValue = new QLineEdit;
    _constValue = new QLineEdit;
    _okButton = new QPushButton;
    _resetButton = new QPushButton;
    _cancelButton = new QPushButton;
    _quadValue->setText(QString::number(_oldValues[0]));
    _linValue->setText(QString::number(_oldValues[1]));
    _constValue->setText(QString::number(_oldValues[2]));
    _okButton->setText("Ok");
    _resetButton->setText("Reset");
    _cancelButton->setText("Cancel");
    _cancelButton->setDefault(true);
    _layout->addWidget(_quadLabel, 0, 0, Qt::AlignHCenter);
    _layout->addWidget(_linLabel, 0, 1, Qt::AlignHCenter);
    _layout->addWidget(_constLabel, 0, 2, Qt::AlignHCenter);
    _layout->addWidget(_quadValue, 1, 0, Qt::AlignHCenter);
    _layout->addWidget(_linValue, 1, 1, Qt::AlignHCenter);
    _layout->addWidget(_constValue, 1, 2, Qt::AlignHCenter);
    _layout->addWidget(_okButton, 2, 0, Qt::AlignHCenter);
    _layout->addWidget(_resetButton, 2, 1, Qt::AlignHCenter);
    _layout->addWidget(_cancelButton, 2, 2, Qt::AlignHCenter);
    setLayout(_layout);
    adjustSize();

    connect(_okButton, SIGNAL(clicked()), SLOT(accept()));
    connect(_resetButton, SIGNAL(clicked()), SLOT(ResetValues()));
    connect(_cancelButton, SIGNAL(clicked()), SLOT(close()));
}

void SetCalibrationValuesDialog::ResetValues()
{
   _quadValue->setText(QString::number(_oldValues[0], 10, 4));
   _linValue->setText(QString::number(_oldValues[1], 10, 4));
   _constValue->setText(QString::number(_oldValues[2], 10, 4));
}

SetCalibrationValuesDialog::~SetCalibrationValuesDialog()
{
   delete _layout;
}
