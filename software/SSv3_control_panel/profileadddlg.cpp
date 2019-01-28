#include "profileadddlg.h"

ProfileAddDlg::ProfileAddDlg(QWidget *parent)
    : QDialog(parent)
{
   setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
    setupUi(this);
}

ProfileAddDlg::~ProfileAddDlg()
{
}

void ProfileAddDlg::on_clearButton_clicked()
{
   radio405->setChecked(false);
   spin405->setValue(0);
   radio445->setChecked(false);
   spin445->setValue(0);
   radio488->setChecked(false);
   spin488->setValue(0);
   radio515->setChecked(false);
   spin515->setValue(0);
   radio535->setChecked(false);
   spin535->setValue(0);
   radio560->setChecked(false);
   spin560->setValue(0);
   radio642->setChecked(false);
   spin642->setValue(0);
}