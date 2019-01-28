#include "generaldialog.h"

GeneralDialog::GeneralDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
    _layout = new QVBoxLayout();
    _label = new QLabel();
    _button = new QPushButton();
    _layout->addWidget(_label);
    _button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _layout->addWidget(_button);
    setLayout(_layout);
    connect(_button, SIGNAL(clicked()), SLOT(close()));
}

GeneralDialog::~GeneralDialog()
{
}

void GeneralDialog::SetText(std::string text)
{
   _label->setText(QString::fromStdString(text));
   _label->update();
   _label->updateGeometry();
}

void GeneralDialog::SetButtonText(std::string text)
{
   _button->setText(QString::fromStdString(text));
}

void GeneralDialog::AddCancel()
{
   _layout->removeWidget(_button);
   _cancelButton = new QPushButton();
   _buttonLayout = new QHBoxLayout();
   _cancelButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
   _cancelButton->setText("Cancel");
   _cancelButton->setDefault(true);
   connect(_cancelButton, SIGNAL(clicked()), SLOT(close()));
   _buttonLayout->addWidget(_button);
   _buttonLayout->addWidget(_cancelButton);
   _layout->addLayout(_buttonLayout);
}