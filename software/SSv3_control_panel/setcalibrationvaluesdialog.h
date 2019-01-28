#pragma once

#include <QDialog>
#include <qlineedit.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include "ui_setcalibrationvaluesdialog.h"

class SetCalibrationValuesDialog : public QDialog, public Ui::SetCalibrationValuesDialog
{
    Q_OBJECT

public:
   SetCalibrationValuesDialog(QWidget *parent = Q_NULLPTR, std::vector<double> current = { 0 });
    ~SetCalibrationValuesDialog();

   QLabel *_quadLabel, *_linLabel, *_constLabel;
   QLineEdit *_quadValue, *_linValue, *_constValue;
   QPushButton *_okButton, *_resetButton, *_cancelButton;
   QGridLayout *_layout;
   double _oldValues[3]{ 0 };

public slots:
   void ResetValues();
};
