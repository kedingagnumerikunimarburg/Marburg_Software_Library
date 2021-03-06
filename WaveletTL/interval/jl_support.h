// -*- c++ -*-

// +--------------------------------------------------------------------+
// | This file is part of WaveletTL - the Wavelet Template Library      |
// |                                                                    |
// | Copyright (c) 2002-2009                                            |
// | Thorsten Raasch, Manuel Werner                                     |
// +--------------------------------------------------------------------+

#ifndef _WAVELETTL_JL_SUPPORT_H
#define _WAVELETTL_JL_SUPPORT_H

#include <list>
#include <set>
#include <interval/jl_basis.h>

namespace WaveletTL
{
  typedef JLBasis::Index Index;

  /*!
    Compute an interval 2^{-j}[k1,k2] which contains the support of a
    single primal [JL] generator or wavelet \psi_\lambda.
    (j == lambda.j()+lambda.e() is neglected for performance reasons)
  */
  void support(const JLBasis& basis,
   	       const Index& lambda,
  	       int& k1, int& k2);
  
  /*!
    Decide whether the support of a generator/wavelet \psi_\lambda
    has a nontrivial intersection with a given dyadic interval 2^{-m}[a,b]
    and compute it in the form 2^{-j}[k1,k2]. If the return value is false,
    k1 and k2 will have no meaningful value for performance reasons.
  */
  bool intersect_supports(const JLBasis& basis,
 			  const Index& lambda,
 			  const int m, const int a, const int b,
 			  int& j, int& k1, int& k2);
  
  /*!
    Decide whether the supports of two generators/wavelets \psi_\lambda and
    \psi_\nu have an intersection of positive measure and compute it
    in the form 2^{-j}[k1,k2]. If the return value is false, the computed
    support intersection will have no meaningful values, for performance reasons.
  */
  bool intersect_supports(const JLBasis& basis,
 			  const Index& lambda,
 			  const Index& nu,
 			  JLBasis::Support& supp);

  /*!
    For a given wavelet \psi_\lambda, compute all generators/wavelets
    \psi_\nu with level |\nu|=j, such that the respective supports
    have a nontrivial intersection
  */
  void intersecting_wavelets(const JLBasis& basis,
 			     const Index& lambda,
 			     const int j, const bool generators,
 			     std::list<Index>& intersecting);
  
  /*!
    For a given wavelet \psi_\lambda, compute all generators/wavelets
    \psi_\nu with level |\nu|=j, such that the respective supports
    have a nontrivial intersection;
    return those intersections
  */
  void intersecting_wavelets(const JLBasis& basis,
 			     const Index& lambda,
 			     const int j, const bool generators,
 			     std::list<std::pair<Index, JLBasis::Support> >& intersecting);

  /*!
    Decide whether the support of a given (primal) generator/wavelet \psi_\lambda
    intersects the singular support of a union 2^{-m}[a,b] of dyadic unit intervals.
    If this is the case, return true and the intersection of \supp\psi_\lambda
    and \supp\psi_\nu in the form 2^{-j}[k1,k2].
    Otherwise, return false (in this case, j, k1 and k2 will have
    no meaningful values, for performance reasons).
    The routine should only be called in the case lambda.j()+lambda.e() >= m.
  */
  bool intersect_singular_support(const JLBasis& basis,
				  const JLBasis::Index& lambda,
				  const int m, const int a, const int b,
				  int& j, int& k1, int& k2);

  /*!
    Decide whether the support of a given (primal) generator/wavelet \psi_\lambda
    intersects the singular support of another (primal) generator/wavelet \psi_\nu.
    If this is the case, return true and the intersection of \supp\psi_\lambda
    and \supp\psi_\nu in the form 2^{-j}[k1,k2].
    Otherwise, return false (in this case, j, k1 and k2 will have
    no meaningful values, for performance reasons).
  */
  bool intersect_singular_support(const JLBasis& basis,
				  const JLBasis::Index& lambda,
				  const JLBasis::Index& nu);

  /*!
    Decide whether the support of a given (primal) generator/wavelet \psi_\lambda
    intersects the singular support of another (primal) generator/wavelet \psi_\nu.
    If this is the case, return true and the intersection of \supp\psi_\lambda
    and \supp\psi_\nu in the form 2^{-j}[k1,k2].
    Otherwise, return false (in this case, j, k1 and k2 will have
    no meaningful values, for performance reasons).
  */
  bool intersect_singular_support(const JLBasis& basis,
				  const JLBasis::Index& lambda,
				  const JLBasis::Index& nu,
				  int& j, int& k1, int& k2);

}

#include <interval/jl_support.cpp>

#endif
