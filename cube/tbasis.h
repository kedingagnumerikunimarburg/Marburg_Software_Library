// -*- c++ -*-

// +--------------------------------------------------------------------+
// | This file is part of WaveletTL - the Wavelet Template Library      |
// |                                                                    |
// | Copyright (c) 2002-2009                                            |
// | Thorsten Raasch, Manuel Werner, Ulrich Friedrich                   |
// +--------------------------------------------------------------------+

#ifndef TBASIS_H_
#define TBASIS_H_

#include <list>
//#include <algebra/vector.h>

#include <algebra/infinite_vector.h>
#include <utils/fixed_array1d.h>
#include <utils/multiindex.h>
#include "tbasis_index.h"
#include <utils/function.h>
#include <geometry/point.h>
#include <utils/array1d.h>

// for convenience, include also some functionality
#include "tbasis_support.h"
//#include "tbasis_evaluate.h"

using std::list;
using MathTL::Point;
using MathTL::Function;
using MathTL::FixedArray1D;
using MathTL::MultiIndex;
using MathTL::InfiniteVector;
using MathTL::Array1D;

namespace WaveletTL
{
    /*
     * Template class for tensor product wavelet bases on the d-dimensional unit cube [0,1]^d (or mapped versions thereof) of the type
     * (V_0 \oplus W_0 \oplus W_1 \oplus W_2 \oplus ...)\times (V_0 \oplus W_0 \oplus W_1 \oplus W_2 \oplus ...)
     * For each of the 2*d facets, you can specify the orders s_i, sT_i of the Dirichlet boundary conditions of
     * the primal and dual basis in the normal direction (this is tailored for the DSBasis constructor...).
     */
    template <class IBASIS, unsigned int DIM = 2>
    class TensorBasis
    {
    public:
        //! Default constructor (no b.c.'s)
        TensorBasis();

        /* Constructor with specified boundary condition orders
         * i-th direction at x=0 <-> index 2*i
         * i-th direction at x=1 <-> index 2*i+1
         */
        /* works only with DS Basis, not with PBasis
    	TensorBasis(const FixedArray1D<int,2*DIM>& s, const FixedArray1D<int,2*DIM>& sT);
         */

    	/*
    	 * Constructor with specified boundary condition orders
    	 * i-th direction at x=0 <-> index 2*i
    	 * i-th direction at x=1 <-> index 2*i+1
    	 */
    	TensorBasis(const FixedArray1D<int,2*DIM>& s);

    	/*
    	 * Constructor with specified Dirichlet boundary conditions for
    	 * the primal functions, the dual functions will be constructed to
    	 * fulfill free b.c.'s
    	 */
    	TensorBasis(const FixedArray1D<bool,2*DIM>& bc);

    	/*
    	 * Constructor with precomputed instances of the 1D bases;
    	 * in this case, the pointers are reused and
    	 * not deleted at destruction time.
    	 */
        TensorBasis(const FixedArray1D<IBASIS*,DIM> bases);

    	//! Destructor
    	~TensorBasis();

    	//! Interval basis
    	typedef IBASIS IntervalBasis;

    	//! Coarsest possible level j0
    	inline const MultiIndex<int,DIM> j0() const { return j0_; }

    	void set_jmax(const MultiIndex<int,DIM> jmax) {
      		jmax_ = jmax;
      		setup_full_collection();
    	}

    	//! Wavelet index class
    	typedef TensorIndex<IBASIS,DIM,TensorBasis<IBASIS,DIM> > Index;

    	//! Read access to the bases
    	const FixedArray1D<IBASIS*,DIM>& bases() const { return bases_; }

    	/*
    	 * Geometric type of the support sets
    	 */
    	typedef struct {
      		int j[DIM];
      		int a[DIM];
      		int b[DIM];
    	} Support;


        /*
         * For a given interval basis IBASIS, compute a cube
         * 2^{-j_}<a_,b_> = 2^{-j_1}[a_1,b_1]x...x2^{-j_n}[a_n,b_n]
         * which contains the support of a single primal cube generator
         * or wavelet psi_lambda.
         */
    	void support(const Index& lambda, Support& supp) const;

    	/*
    	 * Critical Sobolev regularity for the primal generators/wavelets.
    	 * We assume the same regularity in each dimension.
    	 */
    	static double primal_regularity() { return IBASIS::primal_regularity(); }

    	/*
    	 * Degree of polynomial reproduction for the primal generators/wavelets
    	 * We assume the same polynomial degree in each dimension.
    	 * */
    	static unsigned int primal_polynomial_degree() { return IBASIS::primal_polynomial_degree(); }

    	/*
    	 * Number of vanishing moments for the primal wavelets.
    	 * We assume the same number of vanishing moments in each dimension.
    	 */
    	static unsigned int primal_vanishing_moments() { return IBASIS::primal_vanishing_moments(); }

    	//! Index of first generator on level j0
    	Index first_generator() const;

	    //! Index of last generator on level j0
    	Index last_generator() const;

    	//! Index of first wavelet on level j >= j0
    	Index first_wavelet(const MultiIndex<int,DIM> j) const;

    	//! Index of last wavelet on level j >= j0
    	Index last_wavelet(const MultiIndex<int,DIM> j) const;

    	/*
    	 * For a given function, compute all integrals w.r.t. the primal
    	 * or dual generators/wavelets \psi_\lambda with |\lambda|\le jmax.
    	 * - When integrating against the primal functions, the integrand has to be smooth
    	 *   to be accurately reproduced by the dual basis.
    	 * - When integration against dual functions is specified,
    	 *   we integrate against the primal ones instead and multiply the resulting
    	 *   coefficients with the inverse of the primal gramian.
    	 *
    	 * Maybe a thresholding of the returned coefficients is helpful (e.g. for
    	 * expansions of spline functions).
    	 */
    	void expand(const Function<DIM>* f,
                    const bool primal,
                    const MultiIndex<int,DIM> jmax,
                    InfiniteVector<double,Index>& coeffs) const;

    	/*
    	 * Helper function, integrate a smooth function f against a
    	 * primal generator or wavelet
    	 */
    	double integrate(const Function<DIM>* f,
                         const Index& lambda) const;

    	//! Point evaluation of (derivatives of) primal generators or wavelets \psi_\lambda
    	double evaluate(const unsigned int derivative,
                        const Index& lambda,
                        const Point<DIM> x) const;

    	//! Setup full collection of wavelets between j0_ and jmax_ as long as a jmax_ has been specified
    	void setup_full_collection();

    	//! Collection of all wavelets between coarsest and finest level
    	Array1D<Index> full_collection;

    	//! Number of wavelets between coarsest and finest level
    	const int degrees_of_freedom() const { return full_collection.size(); };

    	//! Get the wavelet index corresponding to a specified number
    	const inline Index* get_wavelet (const int number) const {
      		return &full_collection[number];
    	}

    protected:
        //! Coarsest possible level j0
    	MultiIndex<int,DIM> j0_;
    	//int j0_[DIM];

    	//! Finest possible level j0
    	MultiIndex<int,DIM> jmax_;
    	//int jmax_[DIM];

    	/*
    	 * The instances of the 1D bases (in general, we will of course
    	 * need strictly less than DIM instances)
    	 */
    	list<IBASIS*> bases_infact;

    	//! For faster access, all relevant pointers to the 1D bases
    	FixedArray1D<IBASIS*,DIM> bases_;

    	//! Flag for deletion of the pointers
    	bool delete_pointers;
  	};
}

#include "tbasis.cpp"

#endif /*TBASIS_H_*/
