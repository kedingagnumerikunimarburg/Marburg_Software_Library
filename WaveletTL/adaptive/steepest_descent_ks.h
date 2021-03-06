// -*- c++ -*-

// +--------------------------------------------------------------------+
// | This file is part of FrameTL - the Wavelet Template Library        |
// |                                                                    |
// | Copyright (c) 2002-2010                                            |
// | Thorsten Raasch, Manuel Werner , Philipp Keding, Alexander Sieber  |
// +--------------------------------------------------------------------+

#ifndef _WAVELET_TL_STEEPEST_DESCENT_KS_H
#define _WAVELET_TL_STEEPEST_DESCENT_KS_H

#include <algebra/infinite_vector.h>

namespace WaveletTL
{
  
  /*!
    \file steepest_descent.h
    The adaptive steepest descent wavelet frame algorithm from Dahlke,
    Fornasier, Raasch, Stevenson, Werner 2007
   */

  /*!
    \brief
    Adaptive steepest descent wavelet frame algorithm from Dahlke, Fornasier, Raasch,
    Stevenson, Werner 2007.
    \param P The cached discrete problem.
    \param epsilon The target \f$\ell_2\f$-accuracy of the algorithm.
    \param approximations We return the discrete approximations.    
  */
  template <class PROBLEM>
  void steepest_descent_ks_SOLVE(const PROBLEM& P, const double epsilon,
			      InfiniteVector<double, typename PROBLEM::Index>& approximations);
  
  template <class PROBLEM>
  void steepest_descent_ks_QUARKLET_SOLVE(const PROBLEM& P, const double epsilon,
			      InfiniteVector<double, typename PROBLEM::Index>& approximations,
                              const CompressionStrategy strategy = DKR, const double a = 2, const double b = 2);
  
  template <class PROBLEM>
  void steepest_descent_ks_QUARKLET_SOLVE(const PROBLEM& P, const double epsilon,
			      InfiniteVector<double, int>& approximations,
                              const CompressionStrategy strategy = DKR, const double a = 2, const double b = 2);
  
}

#include <adaptive/steepest_descent_ks.cpp>

#endif
