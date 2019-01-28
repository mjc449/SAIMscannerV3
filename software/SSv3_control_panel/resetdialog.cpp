#include "resetdialog.h"

ResetDialog::ResetDialog(QWidget *parent)
    : QDialog(parent)
{
   setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
    setupUi(this);
}

ResetDialog::~ResetDialog()
{
}
