// -*- c++ -*-

// +--------------------------------------------------------------------+
// | This file is part of WaveletTL - the Wavelet Template Library      |
// |                                                                    |
// | Copyright (c) 2002-2009                                            |
// | Thorsten Raasch, Manuel Werner                                     |
// +--------------------------------------------------------------------+

#ifndef _WAVELETTL_BOUNDARY_GRAMIAN_H
#define _WAVELETTL_BOUNDARY_GRAMIAN_H

#include <algebra/matrix.h>
#include <Rd/cdf_utils.h>
#include <Rd/cdf_basis.h>

using MathTL::Matrix;

namespace WaveletTL
{
  /*!
    Assume that phi, phiT are refinable functions on R,
    with masks MASK1/MASK2 and with compact supports [l1,l2]\subset [l1T,l2T].
    Moreover, in this routine, phi and phiT are assumed to be biorthogonal.

    The following routine computes the n-times-n gramian matrix
      Gamma_L = <phi^L_{0,k},phiT^L_{0,l}>_{k,l=-l1T-m-n,...,-l1T-m-1}
    between n boundary functions that satisfy a refinement relation of the form
      (phi^L_{0,k})_{k=-l1T-m-n,...,-l1T-m-1}
      = M_L^T *
        (phi^L_{0,-l1T-m-n}(2.)
         .
         .
         phi^L_{0,-l1T-m-1}(2.)
         phi_{0,-l1T-m}(2.)
         .
         .
         phi_{0,l2T-2*l1T+2*m-2}(2.))
	 
    We assume that MLT is larger than ML, but has the same number of columns.
  */
  template <class MASK1, class MASK2>
  void
  compute_biorthogonal_boundary_gramian(const Matrix<double>& ML,
					const Matrix<double>& MLT,
					Matrix<double>& GammaL);
}

#include <interval/boundary_gramian.cpp>

#endif
