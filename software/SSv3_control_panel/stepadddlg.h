#pragma once

#include <QDialog>
#include "ui_stepadddlg.h"

class stepadddlg : public QDialog, public Ui::stepadddlg
{
    Q_OBJECT

public:
    stepadddlg(QWidget *parent = Q_NULLPTR);
    ~stepadddlg();

};
