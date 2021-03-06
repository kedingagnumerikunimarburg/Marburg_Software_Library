#include <iostream>
#include <list>
#include <string>
#include <algebra/block_matrix.h>
#include <algebra/matrix.h>
#include <algebra/vector.h>

using std::cout;
using std::endl;
using namespace MathTL;

int main()
{
  cout << "Testing the class BlockMatrix ..." << endl;

  BlockMatrix<double> defaultM;
  cout << "- a default block matrix:" << endl << defaultM;

  BlockMatrix<double> N(1);
  cout << "- a BlockMatrix(1):" << endl << N;

  BlockMatrix<double> M(1,2);
  cout << "- a BlockMatrix(1,2) M:" << endl << M;

  Matrix<double>* Mblock = new Matrix<double>(2, 3, "1 2 3 4 5 6");
  M.set_block(0, 0, Mblock);
  M.resize_block_row(0, Mblock->row_dimension());
  M.resize_block_column(0, Mblock->column_dimension());
  Mblock = new Matrix<double>(2, 2, "7 8 9 10");
  M.set_block(0, 1, Mblock);
  M.resize_block_column(1, Mblock->column_dimension());
  cout << "- the same block matrix M after 2 * set_block(0,*,*):" << endl << M;

  BlockMatrix<double>* Mcopy = new BlockMatrix<double>(M);
  BlockMatrix<double>* Mcopy2 = new BlockMatrix<double>(*Mcopy);
  delete Mcopy;
  cout << "- testing copy constructor:" << endl << (*Mcopy2);
  delete Mcopy2;

  Vector<double> x(5, "1 2 3 4 5"), y(2);
  M.apply(x, y);
  cout << "- a vector x=" << x << ", Mx=" << y << endl;

  cout << "- M^T:" << endl << transpose(M);

  x.resize(2);
  x[0] = 1;
  x[1] = 2;
  y.resize(5);
  M.apply_transposed(x, y);
  cout << "- a vector x=" << x << ", (M^T)x=" << y << endl;

  BlockMatrix<double> B(2,3), C(3,3);
  Matrix<double>* B00 = new Matrix<double>(1, 1, "1");
  B.set_block(0, 0, B00);  
  Matrix<double>* B01 = new Matrix<double>(1, 1, "2");
  B.set_block(0, 1, B01);  
//   Matrix<double>* B02 = new Matrix<double>(1, 1, "3");
//   B.set_block(0, 2, B02);  
  Matrix<double>* B10 = new Matrix<double>(1, 1, "4");
  B.set_block(1, 0, B10);  
  Matrix<double>* B11 = new Matrix<double>(1, 1, "5");
  B.set_block(1, 1, B11);  
  Matrix<double>* B12 = new Matrix<double>(1, 1, "6");
  B.set_block(1, 2, B12);  
  cout << "- a BlockMatrix B:" << endl << B;
//   Matrix<double>* C00 = new Matrix<double>(1, 1, "1");
//   C.set_block(0, 0, C00);  
  Matrix<double>* C01 = new Matrix<double>(1, 1, "2");
  C.set_block(0, 1, C01);  
  Matrix<double>* C02 = new Matrix<double>(1, 1, "3");
  C.set_block(0, 2, C02);  
  Matrix<double>* C10 = new Matrix<double>(1, 1, "4");
  C.set_block(1, 0, C10);  
  Matrix<double>* C11 = new Matrix<double>(1, 1, "5");
  C.set_block(1, 1, C11);  
  Matrix<double>* C12 = new Matrix<double>(1, 1, "6");
  C.set_block(1, 2, C12);  
  Matrix<double>* C20 = new Matrix<double>(1, 1, "7");
  C.set_block(2, 0, C20);  
  Matrix<double>* C21 = new Matrix<double>(1, 1, "8");
  C.set_block(2, 1, C21);  
  Matrix<double>* C22 = new Matrix<double>(1, 1, "9");
  C.set_block(2, 2, C22);  
  cout << "- a BlockMatrix C:" << endl << C;
  cout << "- B*C=" << endl << B*C;

  BlockMatrix<double> X(2,3), Y(3,2);
  Matrix<double>* X00 = new Matrix<double>(1, 1, "1");
  X.set_block(0, 0, X00);  
  Matrix<double>* X01 = new Matrix<double>(1, 2, "2 3");
  X.set_block(0, 1, X01);  
  Matrix<double>* X02 = new Matrix<double>(1, 3, "4 5 6");
  X.set_block(0, 2, X02);  
  Matrix<double>* X10 = new Matrix<double>(2, 1, "7 13");
  X.set_block(1, 0, X10);  
  Matrix<double>* X11 = new Matrix<double>(2, 2, "8 9 14 15");
  X.set_block(1, 1, X11);  
  Matrix<double>* X12 = new Matrix<double>(2, 3, "10 11 12 16 17 18");
  X.set_block(1, 2, X12);  
  cout << "- a BlockMatrix X:" << endl << X;

  Matrix<double>* Y00 = new Matrix<double>(1, 1, "1");
  Y.set_block(0, 0, Y00);   
  Matrix<double>* Y01 = new Matrix<double>(1, 2, "2 3");
  Y.set_block(0, 1, Y01);  
  Matrix<double>* Y10 = new Matrix<double>(2, 1, "4 7");
  Y.set_block(1, 0, Y10);  
  Matrix<double>* Y11 = new Matrix<double>(2, 2, "5 6 8 9");
  Y.set_block(1, 1, Y11);  
  Matrix<double>* Y20 = new Matrix<double>(3, 1, "10 13 16");
  Y.set_block(2, 0, Y20);  
  Matrix<double>* Y21 = new Matrix<double>(3, 2, "11 12 14 15 17 18");
  Y.set_block(2, 1, Y21);  

  cout << "- another BlockMatrix Y:" << endl << Y;
  cout << "- X*Y=" << endl << X*Y;

  return 0;
}
