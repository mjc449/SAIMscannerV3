#pragma once

#include <QDialog>
#include "ui_resetdialog.h"

class ResetDialog : public QDialog, public Ui::ResetDialog
{
    Q_OBJECT

public:
    ResetDialog(QWidget *parent = Q_NULLPTR);
    ~ResetDialog();
};
