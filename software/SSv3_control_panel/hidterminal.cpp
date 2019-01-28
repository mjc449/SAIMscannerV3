#include <stdio.h>
#include <sstream>
#include "hidterminal.h"
#include "generaldialog.h"

typedef SSV3::Controller::SSV3ERROR ERR;

HIDTerminal::HIDTerminal(QWidget *parent,
   std::shared_ptr<SSV3::Controller> dev, bool demo)
   : QDialog(parent)
{
   setupUi(this);
   _device = dev;
   _demo = demo;
   _vLayout = new QVBoxLayout;
   _labelsLayout = new QHBoxLayout;
   _inBytesLayout = new QHBoxLayout;
   _outBytesLayout = new QHBoxLayout;
   _responseLayout = new QHBoxLayout;
   _buttonsLayout = new QHBoxLayout;
   _sendButton = new QPushButton();
   _clearButton = new QPushButton();
   _closeButton = new QPushButton();
   _response = new QLabel("Danger Will Robinson!");
   for (int i = 0; i < 64; i++)
   {
      _byteLabels.push_back(new QLabel());
      _byteLabels[i]->setText(QString::number(i));
      _inBytes.push_back(new QLineEdit());
      _outBytes.push_back(new QLineEdit());
      _inBytes[i]->setMaximumWidth(20);
      _outBytes[i]->setMaximumWidth(20);
      _outBytes[i]->setMaxLength(2);
      _outBytes[i]->setInputMask("HH");
      _outBytes[i]->setText("00");
      _inBytes[i]->setReadOnly(true);
      _inBytes[i]->setText("00");
      _labelsLayout->addWidget(_byteLabels[i]);
      _inBytesLayout->addWidget(_inBytes[i]);
      _outBytesLayout->addWidget(_outBytes[i]);
   }
   _vLayout->addLayout(_labelsLayout);
   _vLayout->addLayout(_outBytesLayout);
   _vLayout->addLayout(_inBytesLayout);
   _responseLayout->addStretch();
   _responseLayout->addWidget(_response);
   _responseLayout->addStretch();
   _vLayout->addLayout(_responseLayout);
   _sendButton->setFixedWidth(80);
   _sendButton->setText("Send");
   _sendButton->setDefault(true);
   _clearButton->setFixedWidth(80);
   _clearButton->setText("Clear Input");
   _closeButton->setFixedWidth(80);
   _closeButton->setText("Close");
   _buttonsLayout->addStretch();
   _buttonsLayout->addWidget(_sendButton);
   _buttonsLayout->addWidget(_clearButton);
   _buttonsLayout->addWidget(_closeButton);
   _buttonsLayout->addStretch();
   _vLayout->addLayout(_buttonsLayout);
   setLayout(_vLayout);
   connect(_sendButton, SIGNAL(clicked()), SLOT(on_sendButton_clicked()));
   connect(_clearButton, SIGNAL(clicked()), SLOT(on_clearButton_clicked()));
   connect(_closeButton, SIGNAL(clicked()), SLOT(close()));
   adjustSize();
}

void HIDTerminal::on_sendButton_clicked()
{
   if (_device == nullptr)
   {
      GeneralDialog dialog(this);
      if (dialog.exec())
         return;
   }
   for (int i = 0; i < 64; i++)
   {
      int y;
      std::stringstream s;
      s << _outBytes[i]->text().toStdString();
      s >> std::hex >> y;
      _buffer[i] = y;
   }
   if(!_demo)
      CheckErrors(_device->SendArray(_buffer, 64));
   for (int i = 0; i < 64; i++)
   {
      std::stringstream s;
      int y = _buffer[i];
      s << std::hex << y;
      _inBytes[i]->setText(QString::fromStdString(s.str()));
   }
}

void HIDTerminal::on_clearButton_clicked()
{
   for (int i = 0; i < 64; i++)
      _outBytes[i]->setText("00");
}

HIDTerminal::~HIDTerminal()
{
   delete _vLayout;
}

void HIDTerminal::CheckErrors(ERR error)
{
   if (_demo)
      return;
   QString string;
   const wchar_t *response = _device->HidError();
   if (response != NULL)
   {
      string = QString::fromWCharArray(response);
      _response->setStyleSheet("QLabel {background-color : yellow; color : red; }");
      _response->setText(string);
      return;
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
   case ERR::SSV3ERROR_NO_EXPERIMENT:
      string = "Experiment empty";
      break;
   case ERR::SSV3ERROR_INVALID_LOOP:
      string = "Loop invalid";
      break;
   case ERR::SSV3ERROR_STEP_OUTSIDE_EXPERIMENT_RANGE:
      string = "Invalid experiment step";
      break;
   default:
      string = "Unknown error";
      break;
   }
   if (error == ERR::SSV3ERROR_OK)
   {
      _response->setStyleSheet("QLabel {background-color : white; color : green; }");
      _response->setText(string);
      return ;
   }
   _response->setStyleSheet("QLabel {background-color : yellow; color : red; }");
   _response->setText(string);
   return;

}