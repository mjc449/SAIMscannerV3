#include "errordialog.h"

ErrorDialog::ErrorDialog(QWidget *parent, QString text)
    : QDialog(parent)
{
    setupUi(this);
    setWindowTitle("Error");
    _closeButton = new QPushButton();
    _closeButton->setText("Close");
    _closeButton->setFixedWidth(60);
    _dialogText = new QLabel(text);
    _vLayout = new QVBoxLayout();
    connect(_closeButton, SIGNAL(clicked()), SLOT(close()));
    _vLayout->addWidget(_dialogText);
    _vLayout->addSpacing(60);
    _vLayout->addWidget(_closeButton);
    setLayout(_vLayout);
    adjustSize();
}

ErrorDialog::~ErrorDialog()
{
   delete _vLayout;
}

void ErrorDialog::SetText(std::string text)
{
   _dialogText->setText(QString::fromStdString(text));
}