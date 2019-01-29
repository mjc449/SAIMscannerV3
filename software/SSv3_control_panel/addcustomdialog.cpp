/**/////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                                                                            //
//  Copyright(c) 2018, Marshall Colville mjc449@cornell.edu                   //
//  All rights reserved.                                                      //
//                                                                            //
//  Redistribution and use in source and binary forms, with or without        //
//  modification, are permitted provided that the following conditions are    //
//  met :                                                                     //
//                                                                            //
//  1. Redistributions of source code must retain the above copyright notice, //
//  this list of conditions and the following disclaimer.                     //
//  2. Redistributions in binary form must reproduce the above copyright      //
//  notice, this list of conditions and the following disclaimer in the       //
//  documentation and/or other materials provided with the distribution.      //
//                                                                            //
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       //
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED //
//  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A           //
//  PARTICULAR PURPOSE ARE DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT OWNER   //
//  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,  //
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,       //
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR        //
//  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    //
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING      //
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS        //
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.              //
//                                                                            //
//  The views and conclusions contained in the software and documentation are //
//  those of the authors and should not be interpreted as representing        //
//  official policies, either expressed or implied, of the SAIMScannerV3      //
//  project, the Paszek Research Group, or Cornell University.                //
//////////////////////////////////////////////////////////////////////////////*/

#include "addcustomdialog.h"

AddCustomDialog::AddCustomDialog(QWidget *parent)
    : QDialog(parent)
{
   setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
    setupUi(this);
    linearCustomTab->setCurrentIndex(0);
}

AddCustomDialog::~AddCustomDialog()
{
}

void AddCustomDialog::on_degreesButton_toggled()
{
   for (int i = 0; i < customTable->rowCount(); i++)
      customTable->item(i, 0)->setText("0.00");

   firstAngleSpin->setDecimals(2);
   firstAngleSpin->setValue(0.00);
   firstAngleSpin->setMaximum(70.00);
   firstAngleSpin->setMinimum(0.00);

   lastAngleSpin->setDecimals(2);
   lastAngleSpin->setValue(0.00);
   lastAngleSpin->setMaximum(70.00);
   lastAngleSpin->setMinimum(0.00);
}

void AddCustomDialog::on_dacButton_toggled()
{
   for (int i = 0; i < customTable->rowCount(); i++)
      customTable->item(i, 0)->setText("0");

   firstAngleSpin->setValue(0);
   firstAngleSpin->setDecimals(0);
   firstAngleSpin->setMaximum(0x8000);
   firstAngleSpin->setMinimum(0);

   lastAngleSpin->setValue(0);
   lastAngleSpin->setDecimals(0);
   lastAngleSpin->setMaximum(0x8000);
   lastAngleSpin->setMinimum(0);
}

void AddCustomDialog::on_clearButton_clicked()
{
   if (linearCustomTab->currentIndex() == 0)
   {
      firstAngleSpin->setValue(0);
      lastAngleSpin->setValue(0);
      numberAnglesSpin->setValue(0);
   }
   else if (linearCustomTab->currentIndex() == 1)
   {
      for (int i = 0; i < customTable->rowCount(); i++)
         if (degreesButton->isChecked())
            customTable->item(i, 0)->setText("0.00");
         else
            customTable->item(i, 0)->setText("0");
   }
}

void AddCustomDialog::on_addAngleButton_clicked()
{
   int row{-1};
   row = customTable->currentRow() > row ? customTable->currentRow() + 1 : 0;

   customTable->insertRow(row);
   QTableWidgetItem *item = new QTableWidgetItem;
   if (degreesButton->isChecked())
      item->setText("0.00");
   else
      item->setText("0");
   customTable->setItem(row, 0, item);
   customTable->setCurrentItem(item);
}

void AddCustomDialog::on_deleteAngleButton_clicked()
{
   int selection = customTable->currentRow();
   if (selection < 0)
      return;

   customTable->removeRow(selection);

   if (customTable->rowCount() < 0)
      return;
   else if (selection == 0)
      customTable->setCurrentCell(0, 0);
   else if (selection == customTable->rowCount())
      customTable->setCurrentCell(selection - 1, 0);
   else
      customTable->setCurrentCell(selection, 0);
}