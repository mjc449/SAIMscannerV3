#include "setcalibrationvaluesdialog.h"

SetCalibrationValuesDialog::SetCalibrationValuesDialog(QWidget *parent, std::vector<double> current)
    : QDialog(parent)
{
    setupUi(this);

    for (int i = 0; i < 3; i++)
       _oldValues[i] = current[i];

    setWindowTitle("Update calibration");
    _layout = new QGridLayout();
    _quadLabel = new QLabel("Quadratic");
    _linLabel = new QLabel("Linear");
    _constLabel = new QLabel("Constant");
    _quadValue = new QLineEdit;
    _linValue = new QLineEdit;
    _constValue = new QLineEdit;
    _okButton = new QPushButton;
    _resetButton = new QPushButton;
    _cancelButton = new QPushButton;
    _quadValue->setText(QString::number(_oldValues[0]));
    _linValue->setText(QString::number(_oldValues[1]));
    _constValue->setText(QString::number(_oldValues[2]));
    _okButton->setText("Ok");
    _resetButton->setText("Reset");
    _cancelButton->setText("Cancel");
    _cancelButton->setDefault(true);
    _layout->addWidget(_quadLabel, 0, 0, Qt::AlignHCenter);
    _layout->addWidget(_linLabel, 0, 1, Qt::AlignHCenter);
    _layout->addWidget(_constLabel, 0, 2, Qt::AlignHCenter);
    _layout->addWidget(_quadValue, 1, 0, Qt::AlignHCenter);
    _layout->addWidget(_linValue, 1, 1, Qt::AlignHCenter);
    _layout->addWidget(_constValue, 1, 2, Qt::AlignHCenter);
    _layout->addWidget(_okButton, 2, 0, Qt::AlignHCenter);
    _layout->addWidget(_resetButton, 2, 1, Qt::AlignHCenter);
    _layout->addWidget(_cancelButton, 2, 2, Qt::AlignHCenter);
    setLayout(_layout);
    adjustSize();

    connect(_okButton, SIGNAL(clicked()), SLOT(accept()));
    connect(_resetButton, SIGNAL(clicked()), SLOT(ResetValues()));
    connect(_cancelButton, SIGNAL(clicked()), SLOT(close()));
}

void SetCalibrationValuesDialog::ResetValues()
{
   _quadValue->setText(QString::number(_oldValues[0], 10, 4));
   _linValue->setText(QString::number(_oldValues[1], 10, 4));
   _constValue->setText(QString::number(_oldValues[2], 10, 4));
}

SetCalibrationValuesDialog::~SetCalibrationValuesDialog()
{
   delete _layout;
}
