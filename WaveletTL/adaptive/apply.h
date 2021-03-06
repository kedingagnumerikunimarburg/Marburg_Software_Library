// -*- c++ -*-

// +--------------------------------------------------------------------+
// | This file is part of WaveletTL - the Wavelet Template Library      |
// |                                                                    |
// | Copyright (c) 2002-2009                                            |
// | Thorsten Raasch, Manuel Werner                                     |
// +--------------------------------------------------------------------+

#ifndef _WAVELETTL_APPLY_H
#define _WAVELETTL_APPLY_H

#include <algebra/infinite_vector.h>
#include <adaptive/compression.h>



namespace WaveletTL
{
    using MathTL::InfiniteVector;

  /*!
    Apply the stiffness matrix A of an infinite-dimensional equation

      Au = f

    to a given vector v within a specified target accuracy,

      ||w-Av|| <= eta.
      
    The matrix A is assumed to be s^*-compressible, i.e., for a given k,
    there exists a matrix A_k with at most alpha_k*2^k nonzero entries
    per row and column, so that

      ||A-A_k|| <= alpha_k * 2^{-ks} <= alpha * 2^{-ks}

    for s < s^*. The class PROBLEM has to provide these parameters as routines

      double s_star() const
      double alphak(const unsigned int) const

    An example of the template parameter PROBLEM is the template class
    SturmEquation<WBASIS>.

    The action of APPLY can be restricted to entries below a maximal scale jmax.

    References:
    [B]    Barinka:
           Fast Computation Tools for Adaptive Wavelet Schemes
    [BDD]  Barinka/Dahlke/Dahmen:
           Adaptive Application of Operators in Standard Representation.
    [CDD1] Cohen/Dahmen/DeVore:
           Adaptive Wavelet Methods for Elliptic Operator Equations - Convergence Rates
    [S]    Stevenson:
           Adaptive Solution of Operator Equations using Wavelet Frames
  */
  template <class PROBLEM>
  void APPLY(const PROBLEM& P,
	     const InfiniteVector<double, typename PROBLEM::Index>& v,
	     const double eta,
	     InfiniteVector<double, typename PROBLEM::Index>& w,
	     const int jmax = 99,
	     const CompressionStrategy strategy = St04a);
  
  
  template <class PROBLEM>
  void APPLY_QUARKLET(const PROBLEM& P,
	     const InfiniteVector<double, typename PROBLEM::Index>& v,
	     const double eta,
	     InfiniteVector<double, typename PROBLEM::Index>& w,
	     const int jmax = 99,
	     const CompressionStrategy strategy = DKR,
             const int pmax = 0,
             const double a = 2,
             const double b = 2);
  
  template <class PROBLEM>
  void APPLY_QUARKLET(const PROBLEM& P,
	     const InfiniteVector<double,int>& v,
	     const double eta,
	     InfiniteVector<double,int>& w,
	     const int jmax = 99,
	     const CompressionStrategy strategy = DKR,
             const int pmax = 0,
             const double a = 2,
             const double b = 2);
  
  
  template <class PROBLEM>
  void APPLY_QUARKLET_SEQENTIAL(const PROBLEM& P,
	     const InfiniteVector<double, typename PROBLEM::Index>& v,
	     const double eta,
	     InfiniteVector<double, typename PROBLEM::Index>& w,
	     const int jmax = 99,
	     const CompressionStrategy strategy = DKR,
             const int pmax = 0,
             const double a = 2,
             const double b = 2);
  
  template <class PROBLEM>
  void APPLY_QUARKLET_PARALLEL_OUTER(const PROBLEM& P,
	     const InfiniteVector<double, typename PROBLEM::Index>& v,
	     const double eta,
	     InfiniteVector<double, typename PROBLEM::Index>& w,
	     const int jmax = 99,
	     const CompressionStrategy strategy = DKR,
             const int pmax = 0,
             const double a = 2,
             const double b = 2);
  
  template <class PROBLEM>
  void APPLY_QUARKLET_PARALLEL_INNER(const PROBLEM& P,
	     const InfiniteVector<double, typename PROBLEM::Index>& v,
	     const double eta,
	     InfiniteVector<double, typename PROBLEM::Index>& w,
	     const int jmax = 99,
	     const CompressionStrategy strategy = DKR,
             const int pmax = 0,
             const double a = 2,
             const double b = 2);
  
  template <class PROBLEM>
  void APPLY_QUARKLET_COARSE(const PROBLEM& P,
	     const InfiniteVector<double, typename PROBLEM::Index>& v,
	     const double eta,
	     InfiniteVector<double, typename PROBLEM::Index>& w,
	     const int jmax = 99,
	     const CompressionStrategy strategy = DKR,
             const int pmax = 0,
             const double a = 2,
             const double b = 2,
             const double lambda = 0.5){
      InfiniteVector<double, typename PROBLEM::Index> help;
      APPLY_QUARKLET(P, v, lambda*eta, help, jmax, strategy, pmax, a, b);
      help.COARSE((1-lambda)*eta, w);
  };
  
  
  template <class PROBLEM>
  void APPLY_QUARKLET_COARSE(const PROBLEM& P,
	     const InfiniteVector<double, int>& v,
	     const double eta,
	     InfiniteVector<double, int>& w,
	     const int jmax = 99,
	     const CompressionStrategy strategy = DKR,
             const int pmax = 0,
             const double a = 2,
             const double b = 2,
             const double lambda = 0.5){
      InfiniteVector<double, int> help;
      APPLY_QUARKLET(P, v, lambda*eta, help, jmax, strategy, pmax, a, b);
      help.COARSE((1-lambda)*eta, w);
  };
  
  
  /*
   */
  template <class PROBLEM>
  void APPLY(const PROBLEM& P,
	     const int p,
	     const InfiniteVector<double, typename PROBLEM::Index>& v,
	     const double eta,
	     InfiniteVector<double, typename PROBLEM::Index>& w,
	     const int jmax = 99,
	     const CompressionStrategy strategy = St04a);


  template <class PROBLEM>
  void APPLY_TEST(const PROBLEM& P,
		  const InfiniteVector<double, typename PROBLEM::Index>& v,
		  const double eta,
		  InfiniteVector<double, typename PROBLEM::Index>& w,
		  const int jmax = 99,
		  const CompressionStrategy strategy = St04a);


  template <class PROBLEM>
  void APPLY_OPTIMIZED(const PROBLEM& P,
		       const InfiniteVector<double, typename PROBLEM::Index>& v,
		       const double eta,
		       InfiniteVector<double, typename PROBLEM::Index>& w,
		       double& time,
		       const int jmax = 99,
		       const CompressionStrategy strategy = St04a);



  /*!
    APPLY with successive COARSE.
  */
  template <class PROBLEM>
  void APPLY_COARSE(const PROBLEM& P,
		    const InfiniteVector<double, typename PROBLEM::Index>& v,
		    const double eta,
		    InfiniteVector<double, typename PROBLEM::Index>& w,
		    const double lambda = 0.5,
		    const int jmax = 99,
		    const CompressionStrategy strategy = St04a)
  {
    InfiniteVector<double, typename PROBLEM::Index> help;
    APPLY(P, v, lambda*eta, help, jmax, strategy);
    help.COARSE((1-lambda)*eta, w);
  }
  
  /*!
    Adaptive computation of approxiamte residual
   !!!!!!! ADD COMMENT !!!!!!!
  */
  template <class PROBLEM>
  void RES(const PROBLEM& P,
	   const InfiniteVector<double, typename PROBLEM::Index>& w,
	   const double xi,
	   const double delta,
	   const double epsilon,
	   const int jmax,
	   InfiniteVector<double, typename PROBLEM::Index>& tilde_r,
	   double& nu,
	   unsigned int& niter,
           const CompressionStrategy strategy = St04a,
           const bool apply_coarse = true
	   );



template <class PROBLEM>
  void RES_QUARKLET(const PROBLEM& P,
	   const InfiniteVector<double, typename PROBLEM::Index>& w,
	   const double xi,
	   const double delta,
	   const double epsilon,
	   const int jmax,
	   InfiniteVector<double, typename PROBLEM::Index>& tilde_r,
	   double& nu,
	   unsigned int& niter,
	   const CompressionStrategy strategy = DKR,
           const int pmax = 0,
           const double a = 2,
           const double b = 2
	   );

template <class PROBLEM>
  void RES_QUARKLET(const PROBLEM& P,
	   const InfiniteVector<double, int>& w,
	   const double xi,
	   const double delta,
	   const double epsilon,
	   const int jmax,
	   InfiniteVector<double, int>& tilde_r,
	   double& nu,
	   unsigned int& niter,
	   const CompressionStrategy strategy = DKR,
           const int pmax = 0,
           const double a = 2,
           const double b = 2
	   );

}



#include <adaptive/apply.cpp>

#endif
