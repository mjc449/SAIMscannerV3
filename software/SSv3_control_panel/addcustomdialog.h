#pragma once

#include <QDialog>
#include "ui_addcustomdialog.h"

class AddCustomDialog : public QDialog, public Ui::AddCustomDialog
{
    Q_OBJECT

public:
    AddCustomDialog(QWidget *parent = Q_NULLPTR);
    ~AddCustomDialog();

private slots:
   void on_degreesButton_toggled();
   void on_dacButton_toggled();
   void on_clearButton_clicked();
   void on_addAngleButton_clicked();
   void on_deleteAngleButton_clicked();
};
