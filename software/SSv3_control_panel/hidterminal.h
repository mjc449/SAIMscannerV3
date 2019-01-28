#pragma once

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
