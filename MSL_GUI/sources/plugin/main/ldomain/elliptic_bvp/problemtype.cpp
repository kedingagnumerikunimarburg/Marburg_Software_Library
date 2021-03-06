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


#include "problemtype.h"


namespace ldomain
{
namespace elliptic_bvp
{

void ProblemType::setupNamesAndDescription()
{
    problemDescription_ = QString("Elliptic boundary value problem in divergence form on the L-domain %1 = (-1,1)²\\[0,1)² with homogeneous Dirichlet boundary conditions: "
                                  "Find u(x,y) with\n"
                                  "\n"
                                  "    -div(a(x,y)%2u(x,y)) + q(x,y)u(x,y) = f(x,y),   (x,y) %3 %1    and u(x,y) = 0 on %4%1.")
                          .arg(QChar(0x03A9), QChar(0x2207), QChar(0x2208), QChar(0x2202));
    functionIdentifiers_ = {"a", "q", "f"};
    functionVariables_ = {"x", "y"};
}



void ProblemType::setupExamples()
{

}

}
}
