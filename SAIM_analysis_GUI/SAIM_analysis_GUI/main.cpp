#include "saim_analysis_gui.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SAIM_analysis_GUI w;
    w.show();
    return a.exec();
}
