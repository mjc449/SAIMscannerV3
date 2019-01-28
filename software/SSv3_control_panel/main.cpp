#include "ssv3controlpanel.h"
#include <sstream>
#include <QtWidgets/QApplication>
#include <qmessagebox.h>

int main(int argc, char *argv[])
{
   QApplication a(argc, argv);
   SSv3ControlPanel w;
   w.show();
   try
   {
      a.exec();
   }
   catch (...)
   {
      std::exception_ptr eptr = std::current_exception();
      try
      {
         std::rethrow_exception(eptr);
      }
      catch (const std::exception &e)
      {
         w.DumpLogs(e);
         std::stringstream eStream;
         eStream << "Caught an unknown exception in SSv3 Control Panel: \n"
            << e.what() << "\n"
            << "Please forward this message, any error logs and the actions that caused this exception to mjc449@cornell.edu";
         QMessageBox::StandardButton kill = QMessageBox::critical(NULL, QString("Exception caught"), QString::fromStdString(eStream.str()));
      }
      return 0;
   }
}
