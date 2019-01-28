#pragma once

#include <QDialog>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include "ui_generaldialog.h"

class GeneralDialog : public QDialog, public Ui::HIDNotConnected
{
    Q_OBJECT

public:
    GeneralDialog(QWidget *parent = Q_NULLPTR);
    ~GeneralDialog();
    
    void SetText(std::string);
    void SetButtonText(std::string);
    void AddCancel();

    QVBoxLayout *_layout;
    QHBoxLayout *_buttonLayout;
    QPushButton *_button;
    QPushButton *_cancelButton;
    QLabel *_label;
};