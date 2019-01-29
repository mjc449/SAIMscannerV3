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

#include "generaldialog.h"

GeneralDialog::GeneralDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
    _layout = new QVBoxLayout();
    _label = new QLabel();
    _button = new QPushButton();
    _layout->addWidget(_label);
    _button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _layout->addWidget(_button);
    setLayout(_layout);
    connect(_button, SIGNAL(clicked()), SLOT(close()));
}

GeneralDialog::~GeneralDialog()
{
}

void GeneralDialog::SetText(std::string text)
{
   _label->setText(QString::fromStdString(text));
   _label->update();
   _label->updateGeometry();
}

void GeneralDialog::SetButtonText(std::string text)
{
   _button->setText(QString::fromStdString(text));
}

void GeneralDialog::AddCancel()
{
   _layout->removeWidget(_button);
   _cancelButton = new QPushButton();
   _buttonLayout = new QHBoxLayout();
   _cancelButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
   _cancelButton->setText("Cancel");
   _cancelButton->setDefault(true);
   connect(_cancelButton, SIGNAL(clicked()), SLOT(close()));
   _buttonLayout->addWidget(_button);
   _buttonLayout->addWidget(_cancelButton);
   _layout->addLayout(_buttonLayout);
}