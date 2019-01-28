#pragma once

#include <QWidget>
#include <QCloseEvent>
#include <qdialog.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qtextedit.h>
#include <qlineedit.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qtimer.h>
#include "ui_loggwindow.h"
#include "ssv3controlpanel.h"


class TextEditStream : public QTextEdit
{
private:
   QTextEdit *_thisEdit;
public:
   TextEditStream() {}
   TextEditStream(QTextEdit *edit)
   {
      _thisEdit = edit;
   }
   TextEditStream(TextEditStream &obj)
   {
      _thisEdit = obj._thisEdit;
   }
   TextEditStream & operator=(const TextEditStream &obj)
   {
      _thisEdit = obj._thisEdit;
      return *this;
   }
   void OutputDest(QTextEdit *obj)
   {
      _thisEdit = obj;
      return;
   }
   TextEditStream & operator<<(const QString &str)
   {
      _thisEdit->insertPlainText(str);
      return *this;
   }
   TextEditStream & operator<<(const std::string &str)
   {
      _thisEdit->insertPlainText(QString::fromStdString(str));
      return *this;
   }
   TextEditStream & operator<<(const int &num)
   {
      _thisEdit->insertPlainText(QString::number(num));
      return *this;
   }
   TextEditStream & operator<<(const double &num)
   {
      _thisEdit->insertPlainText(QString::number(num, 'f', 3));
      return *this;
   }
   TextEditStream & operator<<(const char *str)
   {
      _thisEdit->insertPlainText(QString(str));
      return *this;
   }
};

class SSv3ControlPanel;

class LoggWindow : public QWidget, public Ui::LoggWindow
{
    Q_OBJECT
public:
    LoggWindow(SSv3ControlPanel *cp, QWidget *parent = Q_NULLPTR);
    ~LoggWindow();

private:
   friend class SSv3ControlPanel;

   void SetupWindow();
   void PrintSequence(int seq);
   void PrintProfile(int prof);
   void PrintExperiment();

   virtual void closeEvent(QCloseEvent *event);

   SSv3ControlPanel* _mainWindow;
   QVBoxLayout *_mainLayout;
   QTextEdit *_textBox;
   QTextEdit *_commentBox;

   QLineEdit *_saveLocation;
   QHBoxLayout *_saveLayout;
   QCheckBox *_autosaveCheckBox;
   QPushButton *_saveLocationButton;

   QHBoxLayout *_commentButtonLayout;
   QPushButton *_addCommentButton;
   QPushButton *_clearCommentButton;

   QHBoxLayout *_controlButtonLayout;
   QPushButton *_recordSettingsButton;
   QPushButton *_recordElementsButton;
   QPushButton *_saveAsButton;
   QPushButton *_closeButton;

   TextEditStream _textBoxStream;
   QTextStream _fileStream;
   QFile _logFile;
   QTimer *_autosaveTimer;

private slots:
   void on_saveLocationButton_clicked();
   void on_saveAsButton_clicked();
   void on_autosaveCheckBox_checked();
   void SaveLog();
   void on_addComment();
   void on_recordSettingsButton_clicked();
   void on_recordElementsButton_clicked();
};
