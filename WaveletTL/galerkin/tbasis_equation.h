// -*- c++ -*-

// +--------------------------------------------------------------------+
// | This file is part of WaveletTL - the Wavelet Template Library      |
// |                                                                    |
// | Copyright (c) 2002-2009                                            |
// | Thorsten Raasch, Manuel Werner, Ulrich Friedrich                   |
// +--------------------------------------------------------------------+

#ifndef _WAVELETTL_TBASIS_EQUATION_H
#define	_WAVELETTL_TBASIS_EQUATION_H

#include <set>
#include <utils/fixed_array1d.h>
#include <utils/array1d.h>
#include <numerics/bvp.h>

#include <galerkin/galerkin_utils.h>
#include <galerkin/infinite_preconditioner.h>

using MathTL::FixedArray1D;
using MathTL::EllipticBVP;

namespace WaveletTL
{
    template <class IBASIS, unsigned int DIM> class TensorBasis;


    /*!
    This class models the (preconditioned) infinite-dimensional matrix problem

      Au = D^{-1}LD^{-1}u = D^{-1}F

    when reformulating an elliptic boundary value problem on the cube [0,1]^d

      -div(a(x)grad u(x)) + q(x)u(x) = f(x)

    with first (Dirichlet) or second (Neumann) order b.c.'s (modeled in
    the class EllipticBVP) as an equivalent operator equation
    within \ell_2 by means of a wavelet basis.

    The corresponding bilinear form in

      L = (a(\psi_\nu,\psi_\lambda))_{\lambda,\nu}

    is

      a(u,v) = \int_Omega [a(x)grad u(x)grad v(x)+q(x)u(x)v(x)] dx

    and the right-hand side is

      F(v) = \int_0^1 f(x)v(x) dx

    The evaluation of a(.,.) and f is possible for arguments \psi_\lambda
    which stem from a wavelet basis \Psi=\{\psi_\lambda\} of the corresponding
    function space over Omega.
    \Psi is assumed to be of tensor structure. Unlike in cube_equation it is
    assumed that the minimal level of \Psi is of MultiIndex type, as modeled
    by tbasis. To achieve independence from the concrete choice of \Psi,
    the wavelet basis class is given as a template parameter TENSORBASIS.
    It should have a constructor of the form

       TENSORBASIS::TENSORBASIS(const FixedArray1D<bool,2*DIM>& bc);

    where bc indicates the enforcement of homogeneous Dirichlet boundary conditions (true).
    A natural concrete value for TENSORBASIS is the TensorBasis<DSBasis<d,dT> >.
    */
 
    template <class IBASIS, unsigned int DIM, class TENSORBASIS = TensorBasis<IBASIS,DIM> >
    class TensorEquation
    //     : public FullyDiagonalDyadicPreconditioner<typename TENSORBASIS::Index>
    : public FullyDiagonalEnergyNormPreconditioner<typename TENSORBASIS::Index>
    {
    public:
        /*
         * constructor from a boundary value problem and specified b.c.'s
         */

        TensorEquation(EllipticBVP<DIM>* bvp,
                       const FixedArray1D<bool,2*DIM>& bc,
                       const bool precompute = true);

//     /*!
//     */
//     TensorEquation(const TENSORBASIS* basis,
// 		 const EllipticBVP<DIM>* bvp,
// 		 const FixedArray1D<bool,2*DIM>& bc);


//     /*!
//     */
//     TensorEquation(const TENSORBASIS* basis,
// 		 const EllipticBVP<DIM>* bvp,
// 		 const FixedArray1D<int,2*DIM>& bc);

        /*
         * constructor from a boundary value problem and specified b.c.'s
         */
        TensorEquation(const EllipticBVP<DIM>* bvp,
                       const FixedArray1D<int,2*DIM>& bc,
                       const bool precompute = true);

		/*
		 * Constructor from a boundary value problem and a given tensor basis.
		 * The tensor basis is supposed to incorporate the desired boundary
		 * conditions and a maximal level 'jmax' should have been set to it
		 * beforehand (generally by a call of set_jmax(int)).
		 */
		TensorEquation(EllipticBVP<DIM>* bvp, const TENSORBASIS& basis);

        /*
         * copy constructor
         */
        TensorEquation(const TensorEquation&);

    
        /*
         * make template argument accessible
         */
        typedef TENSORBASIS WaveletBasis;
        typedef IBASIS Basis1D;
        

        /*
         * wavelet index class
         */
        typedef typename WaveletBasis::Index Index;
        typedef typename Basis1D::Index Index1D;

        /*
         * read access to the basis
         */
        const TENSORBASIS& basis() const { return basis_; }

        /*
         * space dimension of the problem
         */
        static const int space_dimension = DIM;

        /*
         * differential operators are local
         */
        static bool local_operator() { return true; }

        /*
         * (half) order t of the operator
         * (inherited from FullyDiagonalEnergyNormPreconditioner)
         */
        double operator_order() const { return 1.; }

        /*
         * evaluate the diagonal preconditioner D
         */
        double D(const typename WaveletBasis::Index& lambda) const;

        /*
         * evaluate the (unpreconditioned) bilinear form a
         * (inherited from FullyDiagonalEnergyNormPreconditioner)
         */
        double a(const Index& lambda,
                 const Index& nu) const;

        /*
         * evaluate the (unpreconditioned) bilinear form a;
         * you can specify the order p of the quadrature rule, i.e.,
         * (piecewise) polynomials of maximal degree p will be integrated exactly.
         * Internally, we use an m-point composite tensor product Gauss rule adapted
         * to the singular supports of the spline wavelets involved,
         * so that m = (p+1)/2;
         */
        double a(const typename WaveletBasis::Index& lambda,
                 const typename WaveletBasis::Index& nu,
                 const unsigned int p) const;
                    
        /*
         * estimate the spectral norm ||A||
         * PERFORMANCE :: use setup_full_collection
         */
        double norm_A() const;

        /*
         * estimate the spectral norm ||A^{-1}||
         */
        double norm_Ainv() const;

        /*
         * estimate compressibility exponent s^*
         * (we assume that the coefficients a(x),q(x) are smooth)
         */
        double s_star() const {return 1.0;}

        /*
         * estimate the compression constants alpha_k in
         * ||A-A_k|| <= alpha_k * 2^{-s*k}
         */
        inline double alphak(const unsigned int k) const {
            return 2*norm_A(); // suboptimal
        }

        /*
         * evaluate the (unpreconditioned) right-hand side f
         */
        double f(const typename WaveletBasis::Index& lambda) const;

        /*
         * approximate the wavelet coefficient set of the preconditioned right-hand side F
         * within a prescribed \ell_2 error tolerance.
         * uses only entries from fcoeffs.
         */
        void RHS(const double eta,
                 InfiniteVector<double,typename WaveletBasis::Index>& coeffs) const;
        void RHS(const double eta,
                 InfiniteVector<double, int>& coeffs) const;

        /*
         * compute (or estimate) ||F||_2
         */
        double F_norm() const { return sqrt(fnorm_sqr); }

        /*
         * set the boundary value problem
         */
        void set_bvp(const EllipticBVP<DIM>*);

        /*
         * set or change the righthandside
         */
        void set_f(const Function<DIM>* fnew);
        
        /*
         * set the maximal wavelet level jmax.
         * modifies
         *   basis_.full_collection, fcoeffs, fnorm_sqr
         */
        inline void set_jmax(const unsigned int jmax, const bool computerhs = true)
        {
            basis_.set_jmax(jmax);
            if (computerhs) compute_rhs();
        }

    protected:
        // #####################################################################################
    // Caching of appearing 1D integrals when making use of the tensor product structure of the wavelets
    // during the evaluation of the bilinear form.
    typedef std::map<Index1D,double > Column1D;
    typedef std::map<Index1D,Column1D> One_D_IntegralCache;
    
    mutable One_D_IntegralCache one_d_integrals;
    // #####################################################################################
        EllipticBVP<DIM>* bvp_;
        TENSORBASIS basis_;
        // right-hand side coefficients on a fine level, sorted by modulus
        Array1D<std::pair<typename WaveletBasis::Index,double> > fcoeffs;
        // precompute the right-hand side
        // TODO PERFORMANCE:: use setup_full_collection entries
        void compute_rhs();
        // (squared) \ell_2 norm of the precomputed right-hand side
        double fnorm_sqr;
        // estimates for ||A|| and ||A^{-1}||
        mutable double normA, normAinv;
    };
}

#include <galerkin/tbasis_equation.cpp>

#endif	/* _WAVELETTL_TBASIS_EQUATION_H */

