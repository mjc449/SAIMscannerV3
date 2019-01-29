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

#ifndef HIDTERMINAL_H
#define HIDTERMINAL_H

#include <QDialog>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <memory>
#include "ui_hidterminal.h"
#include "SSv3_driver\SAIMScannerV3.h"

class HIDTerminal : public QDialog, public Ui::HIDTerminal
{
    Q_OBJECT

public:
    HIDTerminal(QWidget *parent = Q_NULLPTR,
       std::shared_ptr<SSV3::Controller> = nullptr, bool demo = false);
    ~HIDTerminal();

private:
   std::vector<QLineEdit *> _inBytes;
   std::vector<QLineEdit *> _outBytes;
   std::vector<QLabel *> _byteLabels;
   QPushButton *_sendButton;
   QPushButton *_clearButton;
   QPushButton *_closeButton;
   QLabel *_response;
   QHBoxLayout *_labelsLayout, *_inBytesLayout, *_outBytesLayout, *_responseLayout, *_buttonsLayout;
   QVBoxLayout *_vLayout;
   std::shared_ptr<SSV3::Controller> _device;
   unsigned char _buffer[64]{ 0 };
   bool _demo{ false };

   void CheckErrors(SSV3::Controller::SSV3ERROR);

private slots:
   void on_sendButton_clicked();
   void on_clearButton_clicked();
};

#endif //HIDTERMINAL_H