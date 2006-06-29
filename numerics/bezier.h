// -*- c++ -*-

// +--------------------------------------------------------------------+
// | This file is part of MathTL - the Mathematical Template Library    |
// |                                                                    |
// | Copyright (c) 2002-2006                                            |
// | Thorsten Raasch, Manuel Werner                                     |
// +--------------------------------------------------------------------+

#ifndef _MATHTL_BEZIER_H
#define _MATHTL_BEZIER_H

namespace MathTL
{
  /*!
    point evaluation of the k-th Bernstein basis polynomial of degree d

      B_{k,d}(x) = binomial{d}{k} * x^k * (1-x)^{d-k}

    at x, 0<=k<=d.
  */
  template <int d>
  double EvaluateBernsteinPolynomial(const int k, const double x)
  {
    return
      (1-x) * EvaluateBernsteinPolynomial<d-1>(k,  x)
      + x   * EvaluateBernsteinPolynomial<d-1>(k-1,x);
  }

  /*!
    evaluation of B_{k,0}(x)
  */
  template <>
  double EvaluateBernsteinPolynomial<0>(const int k, const double x)
  {
    return (k == 0 ? 1.0 : 0.0);
  }

  /*!
    point evaluation of the first derivative of B_{k,d}(x)
  */
  template <int d>
  double EvaluateBernsteinPolynomial_x(const int k, const double x)
  {
    return d * (EvaluateBernsteinPolynomial<d-1>(k-1, x)
		- EvaluateBernsteinPolynomial<d-1>(k, x));
  }

  /*!
    point evaluation of a cubic polynomial p(x) in Bernstein representation
     
      p(x) = sum_{k=0}^3 B_{k,3}(x) * b_k
  */
  double EvaluateBernsteinPolynomial(const double b0, const double b1,
				     const double b2, const double b3,
				     const double x)
  {
    const double b10 = (1-x)*b0+x*b1;
    const double b11 = (1-x)*b1+x*b2;
    const double b12 = (1-x)*b2+x*b3;
    
    const double b20 = (1-x)*b10+x*b11;
    const double b21 = (1-x)*b11+x*b12;

    return (1-x)*b20+x*b21;
  }
}

#include <numerics/bezier.cpp>

#endif
