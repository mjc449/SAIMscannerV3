#pragma once

#include <QDialog>
#include "ui_profileadddlg.h"

class ProfileAddDlg : public QDialog, public Ui::ProfileAddDlg
{
    Q_OBJECT

public:
    ProfileAddDlg(QWidget *parent = Q_NULLPTR);
    ~ProfileAddDlg();

private slots:
    void on_clearButton_clicked();
};
