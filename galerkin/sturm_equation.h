// -*- c++ -*-

// +--------------------------------------------------------------------+
// | This file is part of WaveletTL - the Wavelet Template Library      |
// |                                                                    |
// | Copyright (c) 2002-2005                                            |
// | Thorsten Raasch                                                    |
// +--------------------------------------------------------------------+

#ifndef _WAVELETTL_STURM_EQUATION_H
#define _WAVELETTL_STURM_EQUATION_H

#include <set>
#include <utils/array1d.h>

#include <galerkin/galerkin_utils.h>

namespace WaveletTL
{
  /*!
    This class models the (preconditioned) infinite-dimensional matrix problem
    
      Au = D^{-1}LD^{-1}u = D^{-1}F

    when reformulating a Sturm boundary value problem on [0,1]
    
      -(py')'(t) + q(t)y(t) = g(t), 0 <= t <= 1 
      
    with first (Dirichlet) or second (Neumann) order b.c.'s as modeled in
    the class SimpleSturmBVP as an equivalent operator equation
    within \ell_2 by means of a wavelet basis.

    The corresponding bilinear form in

      L = (a(\psi_\nu,\psi_\lambda))_{\lambda,\nu}

    is

      a(u,v) = \int_0^1 [p(t)u'(t)v'(t)+q(t)u(t)v(t)] dt
      
    and the right-hand side is
 
      f(v) = \int_0^1 g(t)v(t) dt
   
    The evaluation of a(.,.) and f is possible for arguments \psi_\lambda
    which stem from a wavelet basis \Psi=\{\psi_\lambda\} of the corresponding
    function space over [0,1].
    To achieve independence from the concrete choice of \Psi, the wavelet basis
    class is given as a template parameter WBASIS and should provide a constructor of
    the form

       WBASIS::WBASIS(const bool bc_left, const bool bc_right)

    where the parameters bc_* indicate where to enforce homogeneous Dirichlet
    boundary conditions.
    Of course a natural concrete value for WBASIS is the template class DSBasis<d,dT>.

    References:
    [St04a] Stevenson:
            On the compressibility of operators in wavelet coordinates
  */
  template <class WBASIS>
  class SturmEquation
  {
  public:
    SturmEquation(const SimpleSturmBVP& bvp);

    /*!
      make template argument accessible
    */
    typedef WBASIS WaveletBasis;

    /*!
      wavelet index class
    */
    typedef typename WaveletBasis::Index Index;

    /*!
      read access to the basis
    */
    const WBASIS& basis() const { return basis_; }
    
    /*!
      space dimension of the problem
    */
    static const int space_dimension = 1;

    /*!
      differential operators are local
    */
    static bool local_operator() { return true; }

    /*!
      (half) order t of the operator
    */
    static int operator_order() { return 1; }
    
    /*!
      evaluate the diagonal preconditioner D
    */
    double D(const Index& lambda) const;

    /*!
      rescale a coefficient vector by an integer power of D, c |-> D^{n}c
    */
    void rescale(InfiniteVector<double,Index>& coeffs, const int n) const;

    /*!
      evaluate the (unpreconditioned) bilinear form a;
      you can specify the order p of the quadrature rule, i.e.,
      (piecewise) polynomials of maximal degree p will be integrated exactly.
      Internally, we use an m-point composite Gauss quadrature rule adapted
      to the singular supports of the spline wavelets involved,
      so that m = (p+1)/2;
    */
    double a(const Index& lambda,
	     const Index& nu,
	     const unsigned int p = 4) const;

    /*!
      estimate the spectral norm ||A||
    */
    double norm_A() const;

    /*!
      estimate the spectral norm ||A^{-1}||
    */
    double norm_Ainv() const;

    /*!
      estimate compressibility exponent s^*
    */
    double s_star() const {
      return 1.0 + WBASIS::primal_vanishing_moments(); // [St04a], Th. 2.3 for n=1
    }

    /*!
      estimate the compression constants alpha_k in
        ||A-A_k|| <= alpha_k * 2^{-s*k}
    */
    double alphak(const unsigned int k) const {
      return 2*norm_A(); // suboptimal
    }

    /*!
      evaluate the (unpreconditioned) right-hand side f
    */
    double f(const Index& lambda) const;

    /*!
      approximate the wavelet coefficient set of the preconditioned right-hand side F
      within a prescribed \ell_2 error tolerance
    */
    void RHS(const double eta, InfiniteVector<double,Index>& coeffs) const;

    /*!
      compute (or estimate) ||F||_2
    */
    double F_norm() const { return sqrt(fnorm_sqr); }

  protected:
    const SimpleSturmBVP& bvp_;
    WBASIS basis_;

    // right-hand side coefficients on a fine level, sorted by modulus
    Array1D<std::pair<Index,double> > fcoeffs;

    // (squared) \ell_2 norm of the precomputed right-hand side
    double fnorm_sqr;
  };
}

#include <galerkin/sturm_equation.cpp>

#endif
