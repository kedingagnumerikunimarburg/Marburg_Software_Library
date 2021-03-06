// -*- c++ -*-

// +--------------------------------------------------------------------+
// | This file is part of WaveletTL - the Wavelet Template Library      |
// |                                                                    |
// | Copyright (c) 2002-2009                                            |
// | Thorsten Raasch, Manuel Werner                                     |
// +--------------------------------------------------------------------+

#ifndef _WAVELETTL_P_EVALUATE_H
#define _WAVELETTL_P_EVALUATE_H

#include <geometry/sampled_mapping.h>
#include <algebra/infinite_vector.h>
#include <algebra/piecewise.h>

using MathTL::SampledMapping;
using MathTL::InfiniteVector;
using MathTL::Array1D;
using MathTL::Piecewise;

namespace WaveletTL
{
  template <int d, int dT> class PBasis;

  /*!
    Evaluate a single primal/dual generator or wavelet \psi_\lambda
    on a dyadic subgrid of [0,1].
  */
  template <int d, int dT>
  SampledMapping<1> evaluate(const PBasis<d,dT>& basis,
			     const typename PBasis<d,dT>::Index& lambda,
			     const bool primal,
			     const int resolution);
  
  /*!
    Evaluate an arbitrary linear combination of primal or dual
    wavelets on a dyadic subgrid of [0,1].
  */
  template <int d, int dT>
  SampledMapping<1> evaluate(const PBasis<d,dT>& basis,
			     const InfiniteVector<double, typename PBasis<d,dT>::Index>& coeffs,
			     const bool primal,
			     const int resolution);

  /*!
    point evaluation of (derivatives) of a single primal [P] generator
    or wavelet \psi_\lambda
    The Index version is slow. Usage is disadvised!
  */
  template <int d, int dT>
  double evaluate(const PBasis<d,dT>& basis, const unsigned int derivative,
		  const typename PBasis<d,dT>::Index& lambda,
		  const double x);

  template <int d, int dT>
  double evaluate(const PBasis<d,dT>& basis, const unsigned int derivative,
		  const int j, const int e, const int k,
		  const double x);

  /*!
    Expand of a single primal [P] generator or wavelet \psi_\lambda as a PP Funktion
  */
  template <int d, int dT>
  Piecewise<double> expandAsPP(const PBasis<d,dT>& basis, const typename PBasis<d,dT>::Index& lambda);


  /*!
    point evaluation of (derivatives) of a single primal [P] generator
    or wavelet \psi_\lambda at several points simultaneously
   * 
   * Function using Index is slower than j,e,k version. Usage is disadvised
  */
  template <int d, int dT>
  void evaluate(const PBasis<d,dT>& basis, const unsigned int derivative,
		const typename PBasis<d,dT>::Index& lambda,
		const Array1D<double>& points, Array1D<double>& values);
  template <int d, int dT>
  void evaluate(const PBasis<d,dT>& basis, const unsigned int derivative,
		const int j, const int e, const int k,
		const Array1D<double>& points, Array1D<double>& values);

  /*!
    point evaluation of 0-th and first derivative of a single primal [P] generator
    or wavelet \psi_\lambda at several points simultaneously
  */
  template <int d, int dT>
  void evaluate(const PBasis<d,dT>& basis,
		const typename PBasis<d,dT>::Index& lambda,
		const Array1D<double>& points, Array1D<double>& funcvalues, Array1D<double>& dervalues);  
  
  
  template <int d, int dT>
  void evaluate(const PBasis<d,dT>& basis,
		const int j, const int e, const int k,
		const Array1D<double>& points, Array1D<double>& funcvalues, Array1D<double>& dervalues);
  
  /*! Author: C. Hartmann Date: 30.08.15
    point evaluation of a single primal generator or wavelet \psi_\lambda.
    ATTENTION: This is a direct approach, i.e. we don't expand wavelets into generators
               of a higher level before evaluation.
    !!! ONLY FOR PRIMBS BASIS WITH d=dt=2 AND homog. bc's !!!
  */

  double evaluate_primbs_22_bc11(const int j, const int e, const int k, const double x);

  double evaluate_primbs_22_bc11_v3(const int j, const int e, const int k, const double x);

  double evaluate_primbs_22_bc11_deriv(const int j, const int e, const int k, const double x);

  void evaluate_primbs_22_bc11_v3(const int j, const int e, const int k, const Array1D<double>& points, Array1D<double>& values);


}

#include <interval/p_evaluate.cpp>

#endif
