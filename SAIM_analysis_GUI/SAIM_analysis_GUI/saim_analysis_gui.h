#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_saim_analysis_gui.h"

class SAIM_analysis_GUI : public QMainWindow
{
    Q_OBJECT

public:
    SAIM_analysis_GUI(QWidget *parent = Q_NULLPTR);

private:
    Ui::SAIM_analysis_GUIClass ui;
};
