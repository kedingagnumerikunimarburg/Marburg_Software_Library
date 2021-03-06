// -*- c++ -*-

// +--------------------------------------------------------------------+
// | This file is part of WaveletTL - the Wavelet Template Library      |
// |                                                                    |
// | Copyright (c) 2002-2009                                            |
// | Thorsten Raasch, Manuel Werner                                     |
// +--------------------------------------------------------------------+

#ifndef _WAVELETTL_STURM_EQUATION_H
#define _WAVELETTL_STURM_EQUATION_H

#include <set>
#include <utils/array1d.h>
#include <numerics/sturm_bvp.h>
#include <galerkin/galerkin_utils.h>
#include <galerkin/infinite_preconditioner.h>

using namespace MathTL;

namespace WaveletTL
{

  /*!
    This class models the (preconditioned) infinite-dimensional matrix problem
    
      Au = P^{-1}LQ^{-1}u = P^{-1}F

    when reformulating a Sturm boundary value problem on [0,1]
    
      -(py')'(t) + q(t)y(t) = g(t), 0 <= t <= 1 
      
    with first (Dirichlet) or second (Neumann) order b.c.'s
    (as modeled in the class SimpleSturmBVP)
    as an equivalent operator equation within \ell_2 by means of a
    wavelet basis.

    The corresponding bilinear form in

      L = (a(\psi_\nu,\psi_\lambda))_{\lambda,\nu}

    is

      a(u,v) = \int_0^1 [p(t)u'(t)v'(t)+q(t)u(t)v(t)] dt
      
    and the right-hand side is
 
      f(v) = \int_0^1 g(t)v(t) dt
   
    The evaluation of a(.,.) and f is possible for arguments \psi_\lambda
    which stem from a wavelet basis \Psi=\{\psi_\lambda\} of the corresponding
    function space over [0,1].
    The preconditioners P and Q can either be fully diagonal (P=Q=D)
    or quasi-diagonal (Cholesky decomposition of the "generator block" in L).

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
#ifdef FRAME
  #ifdef DYADIC  
     : public FullyDiagonalQuarkletPreconditioner<typename WBASIS::Index, 1>
  #else
  #ifdef TRIVIAL
  : public TrivialPreconditioner<typename WBASIS::Index>
  #else 
  : public FullyDiagonalEnergyNormPreconditioner<typename WBASIS::Index>
  #endif
  #endif
#endif
  
#ifdef BASIS
#ifdef DYADIC

     : public FullyDiagonalDyadicPreconditioner<typename WBASIS::Index>
#else

    : public FullyDiagonalEnergyNormPreconditioner<typename WBASIS::Index>
#endif
#endif
  {
  public:
    /*!
      make template argument accessible
    */
    typedef WBASIS WaveletBasis;

    SturmEquation(const SimpleSturmBVP& bvp,
		  const bool precompute_rhs = true);

    SturmEquation(const SimpleSturmBVP& bvp,
		  const WaveletBasis& basis,
		  const bool precompute_rhs = true);

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
      (inherited from FullyDiagonalDyadicPreconditioner)
    */
    double operator_order() const { return (bvp_.p(0.0)==1 ? 1. : 0.); }
    
    /*!
      evaluate the diagonal preconditioner D
    */
    double D(const Index& lambda) const;

    /*!
      evaluate the (unpreconditioned) bilinear form a
      (inherited from FullyDiagonalEnergyNormPreconditioner)
    */
    double a(const Index& lambda,
	     const Index& nu) const;

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
	     const unsigned int p) const;

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
      //return 1.5;
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
    
    //int version
    void RHS(const double eta, InfiniteVector<double,int>& coeffs) const;

    /*!
      compute (or estimate) ||F||_2
    */
    double F_norm() const { return sqrt(fnorm_sqr); }

  protected:
    const SimpleSturmBVP& bvp_;
    WBASIS basis_;

    // flag whether right-hand side has already been precomputed
    mutable bool rhs_precomputed;
    
    /*!
      precomputation of the right-hand side
      (constness is not nice but necessary to have RHS a const function)
    */
    void precompute_rhs() const;
    
    //! Precompute the diagonal of the stiffness matrix between minimal and maximal level.
    void compute_diagonal();

    // right-hand side coefficients on a fine level, sorted by modulus
    mutable Array1D<std::pair<Index,double> > fcoeffs;
    mutable Array1D<std::pair<int,double> > fcoeffs_int;
    
    //! Square root of coefficients on diagonal of stiffness matrix.
    Array1D<double> stiff_diagonal;

    // (squared) \ell_2 norm of the precomputed right-hand side
    mutable double fnorm_sqr;

    // estimates for ||A|| and ||A^{-1}||
    mutable double normA, normAinv;
  };
}

#include <galerkin/sturm_equation.cpp>

#endif
