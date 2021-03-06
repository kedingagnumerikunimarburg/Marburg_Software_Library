// -*- c++ -*-

// +--------------------------------------------------------------------+
// | This file is part of WaveletTL - the Wavelet Template Library      |
// |                                                                    |
// | Copyright (c) 2002-2009                                            |
// | Thorsten Raasch, Manuel Werner                                     |
// +--------------------------------------------------------------------+

#ifndef _WAVELETTL_LDOMAIN_BASIS_H
#define _WAVELETTL_LDOMAIN_BASIS_H

#include <map>

#include <algebra/vector.h>
#include <algebra/infinite_vector.h>
#include <algebra/sparse_matrix.h>
#include <algebra/block_matrix.h>
#include <geometry/sampled_mapping.h>
#include <utils/fixed_array1d.h>
#include <utils/multiindex.h>

#include <interval/spline_basis.h>
#include <Ldomain/ldomain_index.h>

// for convenience, include also some functionality
#include <Ldomain/ldomain_support.h>

using std::map;
using MathTL::FixedArray1D;
using MathTL::InfiniteVector;
using MathTL::SparseMatrix;
using MathTL::BlockMatrix;

namespace WaveletTL
{
  /*!
    Template class for composite wavelet bases over the L-shaped domain
      (-1,1)^2 \ (0,1)^2
    such that the primal and the dual basis fulfill homogeneous Dirichlet b.c.'s
    on the outer domain boundary.

    References:
    [DS] Dahmen, Schneider:
         Composite Wavelet Bases for Operator Equations
	 Math. Comput. 68 (1999), 1533-1567
  */
  template <class IBASIS>
  class LDomainBasis
  {
  public:
    //! type of the interval basis
    typedef IBASIS IntervalBasis;

    //! default constructor
    LDomainBasis();

    //! constructor with a precomputed 1D basis
    LDomainBasis(const IntervalBasis& basis1d);

    //! coarsest possible level j0
    inline const int j0() const { return basis1d_.j0(); }
    
    //! wavelet index class
    typedef LDomainIndex<IntervalBasis> Index;

    //! geometric type of the support sets
    typedef struct {
      int j;       // granularity
      int xmin[3];
      int xmax[3];
      int ymin[3];
      int ymax[3];
    } Support;

    //! compute the support of psi_lambda, using the internal cache
    void support(const Index& lambda, Support& supp) const;
    
    //! critical Sobolev regularity for the primal generators/wavelets
    static double primal_regularity() { return IntervalBasis::primal_regularity(); } // dirty, we should use max(1.5,~) instead
    
    //! degree of polynomial reproduction for the primal generators/wavelets
    static unsigned int primal_polynomial_degree() { return IntervalBasis::primal_polynomial_degree(); }

    //! number of vanishing moments for the primal wavelets
    static unsigned int primal_vanishing_moments() { return IntervalBasis::primal_vanishing_moments(); }

    //! read access to the underlying 1D basis
    const IntervalBasis& basis1d() const { return basis1d_; }

    //! size of Delta_j
    const int Deltasize(const int j) const;

    //! sizes of the different wavelet index sets
    const int Nabla01size(const int j) const;
    const int Nabla10size(const int j) const;
    const int Nabla11size(const int j) const;

    /*!
      The following routines provide read access to the diverse refinement (sub)matrices
      on a level j >= j0. The row and column indices follow the less<Index> ordering.
      Those matrices will be collected in an internal cache to provide faster access.
    */
    const BlockMatrix<double>&  get_Mj0      (const int j) const;
    const BlockMatrix<double>&  get_Mj0T     (const int j) const;
    const SparseMatrix<double>& get_Mj1c_1d  (const int j) const; // initial stable completion in 1D
    const BlockMatrix<double>&  get_Mj1c_01  (const int j) const;
    const BlockMatrix<double>&  get_Mj1c_10  (const int j) const;
    const BlockMatrix<double>&  get_Mj1c_11  (const int j) const;

    //! index of first generator on level j >= j0
    Index first_generator(const int j) const;
      
    //! index of last generator on level j >= j0
    Index last_generator(const int j) const;
      
    //! index of first wavelet on level j >= j0
    Index first_wavelet(const int j) const;
      
    //! index of first wavelet on level j >= j0 with type e
    Index first_wavelet(const int j, const typename Index::type_type& e) const;

    //! index of last wavelet on level j >= j0
    Index last_wavelet(const int j) const;

    //! RECONSTRUCT routine, simple version
    /*!
      Constructs for a given single wavelet index lambda a coefficient set c,
      such that
      \psi_lambda = \sum_{\lambda'}c_{\lambda'}\psi_{\lambda'}
      where always |\lambda'|>=j
    */
    void reconstruct_1(const Index& lambda, const int j,
		       InfiniteVector<double, Index>& c) const;

    //! RECONSTRUCT routine, full version
    /*!
      Constructs for a given coefficient set c another one v,
      such that
      \sum_{\lambda}c_\lambda\psi_lambda = \sum_{\lambda'}v_{\lambda'}\psi_{\lambda'}
      where always |\lambda'|>=j
    */
    void reconstruct(const InfiniteVector<double, Index>& c, const int j,
		     InfiniteVector<double, Index>& v) const;

    /*!
      Evaluate a single primal generator or wavelet \psi_\lambda
      on a dyadic subgrid of the L-shaped domain
    */
    Array1D<SampledMapping<2> >
    evaluate
    (const typename LDomainBasis<IntervalBasis>::Index& lambda,
     const int resolution) const;
    
    /*!
      Evaluate an arbitrary linear combination of primal/dual wavelets
      on a dyadic subgrid of the L-shaped domain
    */
    Array1D<SampledMapping<2> >
    evaluate
    (const InfiniteVector<double, typename LDomainBasis<IntervalBasis>::Index>& coeffs,
     const int resolution) const;

    void set_jmax(const int jmax) {
      jmax_ = jmax;
      setup_full_collection();
    }

    //! get the wavelet index corresponding to a specified number
    const inline Index* get_wavelet (const int number) const {
      return &full_collection[number];
    }

    //! number of wavelets between coarsest and finest level
    const int degrees_of_freedom() const { return full_collection.size(); };


  protected:

    //! finest possible level
    int jmax_;

    //! setup full collectin of wavelets between j0_ and jmax_ as long as a jmax_ has been specified
    void setup_full_collection();

    //! collection of all wavelets between coarsest and finest level
    Array1D<Index> full_collection;

    //! the interval 1d wavelet basis
    IntervalBasis basis1d_;

    //! caches for the diverse refinement matrices
    typedef std::map<int,BlockMatrix<double> > MatrixCache;
    mutable MatrixCache Mj0_cache, Mj0T_cache,
      Mj1c_01_cache, Mj1c_10_cache, Mj1c_11_cache;

    typedef std::map<int,SparseMatrix<double> > Matrix1DCache;
    mutable Matrix1DCache Mj1c_1d_cache;

    typedef std::map<Index,InfiniteVector<double,Index> > Mj1Cache;
    mutable Mj1Cache Mj1_cache;
    mutable unsigned long Mj1_hits, Mj1_misses;

    //! support cache
    typedef std::map<Index,Support> SupportCache;
    mutable SupportCache supp_cache;
#if _WAVELETTL_LDOMAINBASIS_VERBOSITY >= 1
    mutable unsigned long supp_hits, supp_misses;
#endif
  };

//   //! template specialization for the case IBASIS==SplineBasis<d,dT,DS_construction>
//   template <int d, int dT>
//   class LDomainBasis<SplineBasis<d,dT,DS_construction> >
//   {
//   public:
//     //! type of the interval basis
//     typedef SplineBasis<d,dT,DS_construction> IntervalBasis;
// 
//     //! size_type, for convenience
//     typedef Vector<double>::size_type size_type;
//     
//     //! constructor with a precomputed 1D basis
//     LDomainBasis(const IntervalBasis& basis1d);
// 
//     //! coarsest possible level j0
//     inline const int j0() const { return basis1d_.j0(); }
//     
//     //! wavelet index class
//     typedef LDomainIndex<IntervalBasis> Index;
// 
//     //! geometric type of the support sets
//     typedef struct {
//       int j;       // granularity
//       int xmin[3];
//       int xmax[3];
//       int ymin[3];
//       int ymax[3];
//     } Support;
// 
//     //! compute the support of psi_lambda, using the internal cache
//     void support(const Index& lambda, Support& supp) const;
//     
//     //! critical Sobolev regularity for the primal generators/wavelets
//     static double primal_regularity() { return IntervalBasis::primal_regularity(); } // dirty, we should use max(1.5,~) instead
//     
//     //! degree of polynomial reproduction for the primal generators/wavelets
//     static unsigned int primal_polynomial_degree() { return IntervalBasis::primal_polynomial_degree(); }
// 
//     //! number of vanishing moments for the primal wavelets
//     static unsigned int primal_vanishing_moments() { return IntervalBasis::primal_vanishing_moments(); }
// 
//     //! read access to the underlying 1D basis
//     const IntervalBasis& basis1d() const { return basis1d_; }
// 
//     //! index of first generator on level j >= j0
//     Index first_generator(const int j) const;
//       
//     //! index of last generator on level j >= j0
//     Index last_generator(const int j) const;
//       
//     //! index of first wavelet on level j >= j0
//     Index first_wavelet(const int j) const;
//       
//     //! index of first wavelet on level j >= j0 with type e
//     Index first_wavelet(const int j, const typename Index::type_type& e) const;
// 
//     //! index of last wavelet on level j >= j0
//     Index last_wavelet(const int j) const;
// 
//     //! size of Delta_j
//     const int Deltasize(const int j) const;
// 
//     //! sizes of the different wavelet index sets
//     const int Nabla01size(const int j) const;
//     const int Nabla10size(const int j) const;
//     const int Nabla11size(const int j) const;
// 
//     //! helper routine: convert a map (of generators on the level j) to an InfiniteVector
//     void map_to_vector(const int j,
// 		       const std::map<size_type,double>& x,
// 		       InfiniteVector<double, Index>& c) const;
// 
//     /*!
//       apply Mj0 to some vector x (i.e. "reconstruct" a generator),
//       the ordering is chosen compatible to Index::number()
//     */
//     void apply_Mj0(const int j,
// 		   const std::map<size_type,double>& x, 
// 		   std::map<size_type,double>& y) const;
// 
//     /*!
//       apply Mj0T^T to some vector x
//     */
//     void apply_Mj0T_transposed(const int j,
// 			       const std::map<size_type,double>& x, 
// 			       std::map<size_type,double>& y) const;
// 
//     /*!
//       apply Mj1c_01 to some vector x (i.e. "reconstruct" a (0,1)-wavelet of the initial completion)
//     */
//     void apply_Mj1c_01(const int j,
// 		       const std::map<size_type,double>& x, 
// 		       std::map<size_type,double>& y) const;
// 
//     /*!
//       apply Mj1c_10 to some vector x (i.e. "reconstruct" a (1,0)-wavelet of the initial completion)
//     */
//     void apply_Mj1c_10(const int j,
// 		       const std::map<size_type,double>& x, 
// 		       std::map<size_type,double>& y) const;
//     
//     /*!
//       apply Mj1c_11 to some vector x (i.e. "reconstruct" a (1,1)-wavelet of the initial completion)
//     */
//     void apply_Mj1c_11(const int j,
// 		       const std::map<size_type,double>& x, 
// 		       std::map<size_type,double>& y) const;
// 
//     
//     //! RECONSTRUCT routine, simple version
//     /*!
//       Constructs for a given single wavelet index lambda a coefficient set c,
//       such that
//       \psi_lambda = \sum_{\lambda'}c_{\lambda'}\psi_{\lambda'}
//       where always |\lambda'|>=j
//     */
//     void reconstruct_1(const Index& lambda, const int j,
// 		       InfiniteVector<double,Index>& c) const;
// 
//     //! reconstruct_1() version which returns two const_iterators (to avoid object copies)
//     void reconstruct_1(const Index& lambda, const int j,
// 		       typename InfiniteVector<double,Index>::const_iterator& it_begin,
// 		       typename InfiniteVector<double,Index>::const_iterator& it_end) const;
// 
//     //! reconstruct_1() version with maps
//     void reconstruct_1(const Index& lambda, const int j,
// 		       std::map<size_type,double>& c) const;
// 
// 
//     /*!
//       Evaluate a single primal generator or wavelet \psi_\lambda
//       on a dyadic subgrid of the L-shaped domain
//     */
//     Array1D<SampledMapping<2> >
//     evaluate
//     (const typename LDomainBasis<IntervalBasis>::Index& lambda,
//      const int resolution) const;
//     
//     /*!
//       Evaluate an arbitrary linear combination of primal/dual wavelets
//       on a dyadic subgrid of the L-shaped domain
//     */
//     Array1D<SampledMapping<2> >
//     evaluate
//     (const InfiniteVector<double, typename LDomainBasis<IntervalBasis>::Index>& coeffs,
//      const int resolution) const;
// 
//     /*!
//       Evaluate a single primal generator or wavelet \psi_\lambda
//       on a tensor product subgrid of a given patch
//       (points are expected to be contained in [0,1])
//     */
//     void
//     evaluate
//     (const typename LDomainBasis<IntervalBasis>::Index& lambda,
//      const int patch,
//      const Array1D<double>& xlist,
//      const Array1D<double>& ylist,
//      Array1D<double>& funcvalues) const;
// 
//     /*!
//       Evaluate the first partial derivatives of a single primal generator
//       or wavelet \psi_\lambda on a tensor product subgrid of a given patch
//       (points are expected to be contained in [0,1])
//     */
//     void
//     evaluate
//     (const typename LDomainBasis<IntervalBasis>::Index& lambda,
//      const int patch,
//      const Array1D<double>& xlist,
//      const Array1D<double>& ylist,
//      Array1D<double>& derxvalues,
//      Array1D<double>& deryvalues) const;
// 
// 
//     void set_jmax(const int jmax) {
//       jmax_ = jmax;
//       setup_full_collection();
//     }
// 
//     //! get the wavelet index corresponding to a specified number
//     const inline Index* get_wavelet (const int number) const {
//       return &full_collection[number];
//     }
// 
//     //! number of wavelets between coarsest and finest level
//     const int degrees_of_freedom() const { return full_collection.size(); };
// 
// 
//   protected:
//     //! finest possible level
//     int jmax_;
// 
//     //! setup full collectin of wavelets between j0_ and jmax_ as long as a jmax_ has been specified
//     void setup_full_collection();
// 
//     //! collection of all wavelets between coarsest and finest level
//     Array1D<Index> full_collection;
// 
// 
//     //! the interval 1d wavelet basis
//     IntervalBasis basis1d_;
// 
//     //! support cache
//     typedef std::map<Index,Support> SupportCache;
//     mutable SupportCache supp_cache;
// #if _WAVELETTL_LDOMAINBASIS_VERBOSITY >= 1
//     mutable unsigned long supp_hits, supp_misses;
// #endif
// 
//     //! reconstruct_1 cache
//     typedef std::map<Index,InfiniteVector<double,Index> > Reconstruct1Cache;
//     mutable Reconstruct1Cache rec1_cache;
// #if _WAVELETTL_LDOMAINBASIS_VERBOSITY >= 1
//     mutable unsigned long rec1_hits, rec1_misses;
// #endif
//   };

}

#include <Ldomain/ldomain_basis.cpp>
#include <Ldomain/ldomain_evaluate.cpp>
#include <Ldomain/ldomain_special.cpp>
#include <Ldomain/ldomain_special_matrices.cpp>

#endif
