// -*- c++ -*-

// +--------------------------------------------------------------------+
// | This file is part of MathTL - the Mathematical Template Library    |
// |                                                                    |
// | Copyright (c) 2002-2009                                            |
// | Thorsten Raasch, Manuel Werner                                     |
// +--------------------------------------------------------------------+

#ifndef _MATHTL_MATRIX_H
#define _MATHTL_MATRIX_H

#include <iostream>
#include <algebra/vector.h>
#include <algebra/matrix_block.h>
#include <algebra/symmetric_matrix.h>
#include <algebra/triangular_matrix.h>
#include <algebra/kronecker_matrix.h>

// matrix norms, for convenience
#include <algebra/matrix_norms.h>

namespace MathTL
{
  /*!
    This class models finite, densely populated matrices
      M = (m_{i,j})_{0<=i<m, 0<=j<n}
    with entries from an arbitrary (scalar) class C,
    designed for numerical computations.
  */
  template <class C>
  class Matrix
    : public MatrixBlock<C>
  {
  public:
    /*!
      type of indexes and size type (cf. STL containers)
     */
    typedef typename Vector<C>::size_type size_type;
    
    /*!
      default constructor, yields zero square matrix which is empty per default
    */
    explicit Matrix(const size_type n = 0);

    /*!
      copy constructor
    */
    Matrix(const Matrix<C>& M);

    /*!
      copy constructor from symmetric matrices
    */
    Matrix(const SymmetricMatrix<C>& M);
    
    /*!
      copy constructor from a lower triangular matrix
    */
    Matrix(const LowerTriangularMatrix<C>& M);
    
    /*!
      copy constructor from an upper triangular matrix
    */
    Matrix(const UpperTriangularMatrix<C>& M);
    
    /*!
      copy constructor from a Kronecker matrix
    */
    template <class MATRIX1, class MATRIX2>
    Matrix(const KroneckerMatrix<C,MATRIX1,MATRIX2>& M);
    
    /*!
      copy constructor from a (column) vector
    */
    Matrix(const Vector<C>& v);

    /*!
      construct m*n rectangular matrix
    */
    Matrix(const size_type row_dimension, const size_type column_dimension);

    /*!
      Construct matrix from a string holding its entries, separated
      by a blank.
      \param row_dimension row dimension
      \param column_dimension column dimension
      \param str input string
      \param byrow indicates whether coefficients are stored row by row in the stream
     */
    Matrix(const size_type row_dimension,
	   const size_type column_dimension,
	   const char* str,
	   const bool byrow = true);

    //! clone the matrix (requirement from MatrixBlock)
    MatrixBlock<C>* clone() const;

    //! transpose the matrix (requirement from MatrixBlock)
    MatrixBlock<C>* clone_transposed() const;

    /*!
      row dimension
    */
    const size_type row_dimension() const;

    /*!
      column dimension
    */
    const size_type column_dimension() const;

    /*!
      size as an STL-compatible container for matrix entries
    */
    const size_type size() const;
    
    /*!
      resize matrix and initialize with zero
    */
    void resize(const size_type rows, const size_type columns);

    /*!
      Matlab reshape functionality: rearrange the entries
      with a given number of rows
      (#rows should divide m*n...)
    */
    void reshape(const size_type rows);

    /*!
      Matlab reshape functionality: rearrange the entries
      as a column vector
    */
    void reshape(Vector<C>& v) const;
    
    /*!
      glue all columns together
    */
    void col(Vector<C>& v) const;

    /*!
      reverse the col() call, with #rows rows
      (#rows should divide v.size()...)
    */
    void decol(const Vector<C>& v, const size_type rows);
    
    /*!
      resize the matrix, prepare an n-by-n diagonal matrix
    */
    void diagonal(const size_type n, const C diag);

    /*!
      (estimate for the) memory consumption in bytes
    */
    const size_type memory_consumption() const;

    /*!
      return true if matrix is empty (cf. STL containers)
    */
    bool empty() const;

    /*!
      read-only access to a matrix entry
    */
    const C operator () (const size_type row, const size_type column) const;

    /*!
      read-only access to a matrix entry
    */
    const C get_entry(const size_type row, const size_type column) const;

    /*!
      read access to a subblock
    */
    template <class MATRIX>
    void get_block(const size_type firstrow, const size_type firstcolumn,
		   const size_type rows, const size_type columns,
		   MATRIX& M, const bool resizeM = false) const;
    
    /*!
      read-write access to a matrix entry
    */
    C& operator () (const size_type row, const size_type column);
    
    /*!
      write access to a matrix entry
    */
    void set_entry(const size_type row, const size_type column, const C value);

    /*!
      write access to a subblock;
      if the "reflect" flag is set, rows and columns of the block are reflected before writing
    */
    template <class MATRIX>
    void set_block(const size_type firstrow, const size_type firstcolumn,
		   const MATRIX& M,
		   const bool reflect = false);

    /*!
      equality test with another matrix
    */
    template <class C2>
    bool operator == (const Matrix<C2>& M) const;

    /*!
      non-equality test
    */
    template <class C2>
    bool operator != (const Matrix<C2>& M) const;

    /*!
      assignment from another matrix
    */
    Matrix<C>& operator = (const Matrix<C>& M);

    /*!
      swap entries of two matrices
    */
    void swap(Matrix<C>& M);

    /*!
      create reflected matrix (flip row and column numbers)
    */
    void reflect(Matrix<C>& M) const;
    
    /*!
      in place scaling *this *= s
    */
    void scale(const C s);

    /*!
      matrix-vector multiplication Mx = (*this) * x;
      we assume that the vector Mx has the correct size and
      is not identical to x
    */
    template <class VECTOR>
    void apply(const VECTOR& x, VECTOR& Mx) const;

    //! special version for Vector<C> (requirement from MatrixBlock)
    void apply(const Vector<C>& x, Vector<C>& Mx) const;

    /*!
      transposed matrix-vector multiplication Mtx = (*this)^T * x;
      we assume that the vector Mtx has the correct size and
      is not identical to x
    */
    template <class VECTOR>
    void apply_transposed(const VECTOR& x, VECTOR& Mtx) const;

    //! special version for Vector<C> (requirement from MatrixBlock)
    void apply_transposed(const Vector<C>& x, Vector<C>& Mtx) const;
    
    /*!
      set all values with modulus below a threshold to zero
      (fabs<C> should exist)
    */
    void compress(const double eta = 1e-16);

    /*!
      stream output with user-defined tabwidth and precision
      (cf. deal.II)
    */
    void print(std::ostream& os,
	       const unsigned int tabwidth = 10,
	       const unsigned int precision = 3) const;

    /*!
      write matrix into Matlab file 
    */
    void matlab_output(const char *file, const char *Matrixname, const int binary) const;

    /*!
      read access to the vector containing the matrix entries in column major ordering
    */
    inline const Vector<C>& entries_vector() const { return entries_; }

  protected:
    /*!
      internal storage of densely populated matrices is just an
      appropriately sized vector, which holds the matrix entries
      in column major ordering
    */
    Vector<C> entries_;

    /*!
      row dimension
    */
    size_type rowdim_;

    /*!
      column dimension
    */
    size_type coldim_;
  };

  /*!
    swap the entries of two matrices
  */
  template <class C>
  void swap(Matrix<C>& M1, Matrix<C>& M2);

  /*!
    matrix-matrix addition M+N
  */
  template <class C>
  Matrix<C> operator + (const Matrix<C>& M, const Matrix<C>& N);

  /*!
    matrix-matrix difference M-N
  */
  template <class C>
  Matrix<C> operator - (const Matrix<C>& M, const Matrix<C>& N);

  /*!
    matrix-matrix multiplication M*N
  */
  template <class C>
  Matrix<C> operator * (const Matrix<C>& M, const Matrix<C>& N);

  /*!
    transpose of a matrix
  */
  template <class C>
  Matrix<C> transpose(const Matrix<C>& M);

  /*!
    Matlab-style stream output for dense matrices
  */
  template <class C>
  std::ostream& operator << (std::ostream& os, const Matrix<C>& M);
}

// include implementation of inline functions
#include <algebra/matrix.cpp>

#endif
