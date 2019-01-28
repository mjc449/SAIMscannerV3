#include "stepadddlg.h"

stepadddlg::stepadddlg(QWidget *parent)
    : QDialog(parent)
{
   setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
    setupUi(this);
}

stepadddlg::~stepadddlg()
{
}
