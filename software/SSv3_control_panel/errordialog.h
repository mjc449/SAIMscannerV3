#pragma once

#include <QDialog>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include "ui_errordialog.h"

class ErrorDialog : public QDialog, public Ui::ErrorDialog
{
    Q_OBJECT

public:
    ErrorDialog(QWidget *parent = Q_NULLPTR, QString text = "Something went wrong!");
    ~ErrorDialog();

    void SetText(std::string);
private:
   QPushButton *_closeButton;
   QLabel *_dialogText;
   QVBoxLayout *_vLayout;
};
