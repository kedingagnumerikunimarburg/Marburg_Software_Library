/*  -*- c++ -*-

   +-----------------------------------------------------------------------+
   | MSL GUI - A Graphical User Interface for the Marburg Software Library |
   |                                                                       |
   | Copyright (C) 2018 Henning Zickermann                                 |
   | Contact: <zickermann@mathematik.uni-marburg.de>                       |
   +-----------------------------------------------------------------------+

     This file is part of MSL GUI.

     MSL GUI is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.

     MSL GUI is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with MSL GUI.  If not, see <https://www.gnu.org/licenses/>.
*/


#ifndef MATRIXNORM_DIALOG_H
#define MATRIXNORM_DIALOG_H

#include <QDialog>

namespace Ui {
class MatrixNormDialog;
}

class MatrixNormDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MatrixNormDialog(QWidget *parent = nullptr);
    ~MatrixNormDialog();

    void resetValuesToOne();
    double getNormA() const;
    double getNormAinv() const;

private slots:
    void on_doubleSpinBox_normA_valueChanged(double value);

    void on_doubleSpinBox_normAinv_valueChanged(double value);

private:
    Ui::MatrixNormDialog* ui_;
};

#endif // MATRIXNORM_DIALOG_H
