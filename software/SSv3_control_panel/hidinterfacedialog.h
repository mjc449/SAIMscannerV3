#pragma once

#include <QDialog>
#include "ui_hidinterfacedialog.h"

class HIDInterfaceDialog : public QDialog, public Ui::HIDInterfaceDialog
{
    Q_OBJECT

public:
    HIDInterfaceDialog(QWidget *parent = Q_NULLPTR);
    ~HIDInterfaceDialog();
};
