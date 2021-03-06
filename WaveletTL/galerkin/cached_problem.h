// -*- c++ -*-

// +--------------------------------------------------------------------+
// | This file is part of WaveletTL - the Wavelet Template Library      |
// |                                                                    |
// | Copyright (c) 2002-2009                                            |
// | Thorsten Raasch, Manuel Werner                                     |
// +--------------------------------------------------------------------+

#ifndef _WAVELETTL_CACHED_PROBLEM_H
#define _WAVELETTL_CACHED_PROBLEM_H

#include <map>
#include <algebra/infinite_vector.h>
#include <algebra/sparse_matrix.h>
#include <adaptive/compression.h>
#include <galerkin/infinite_preconditioner.h>

using MathTL::InfiniteVector;

#ifdef P_POISSON
extern int number_of_entries_computed;
extern int number_of_entries_from_cache;
extern double time_consumption_of_a;
#endif

namespace WaveletTL
{
  /*!
    This class provides a cache layer for generic (preconditioned, cf. precond.h)
    infinite-dimensional matrix problems of the form
    
      Au = P^{-1}LQ^{-1}u = P^{-1}F.

    The operator equation is not assumed to be induced by a local operator,
    i.e., the cache class should also work in the case of integral operators.
    All evaluations of the bilinear form a(.,.) are cached.
    Internally, the cache is managed as follows. The nonzero values of the bilinear
    form a(.,.) are stored in a map.

    The template class CachedProblem implements the minimal signature to be
    used within the APPLY routine.
  */
  template <class PROBLEM>
  class CachedProblem  
#ifdef DYADIC
    : public FullyDiagonalDyadicPreconditioner<typename PROBLEM::Index>
#else
#ifdef TRIVIAL
  : public TrivialPreconditioner<typename PROBLEM::Index>
#else
    : public FullyDiagonalEnergyNormPreconditioner<typename PROBLEM::Index>
#endif    
#endif
  //  : public FullyDyadicPenrichPreconditioner<typename PROBLEM::Index>
  {
  public:
    /*!
      constructor from an uncached problem,
      you can specify the estimates for ||A|| and ||A^{-1}||
      (if zero, CachedProblem will compute the estimates)
    */
    CachedProblem(const PROBLEM* P,
		  const double normA = 0.0,
		  const double normAinv = 0.0);
    
    /*!
      make wavelet basis type accessible
    */
    typedef typename PROBLEM::WaveletBasis WaveletBasis;
    
    /*!
      wavelet index class
    */
    typedef typename PROBLEM::Index Index;
    
    /*!
      read access to the basis
    */
    const WaveletBasis& basis() const { return problem->basis(); }
    
    /*!
      space dimension of the problem
    */
    static const int space_dimension = PROBLEM::space_dimension;
    
    /*!
      locality of the operator
    */
    static bool local_operator() { return PROBLEM::local_operator(); }
    
    /*!
      (half) order t of the operator
    */
    double operator_order() const { return problem->operator_order(); }
    
    /*!
      evaluate the diagonal preconditioner D
    */
    double D(const Index& lambda) const {
#ifdef DYADIC
        return problem->D(lambda);
#endif
        
#ifdef TRIVIAL
       return problem->D(lambda);
#endif

      return sqrt(a(lambda, lambda));  // Christoph: to make use of entries cache!
    }

    /*
     * access to the underlying problem
     */
    const PROBLEM* get_problem() const {return problem;}

    /*!
      evaluate the (unpreconditioned) bilinear form a
      (cached)
    */
    double a(const Index& lambda,
	     const Index& nu) const;
    
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
      (we assume that the coefficients a(x),q(x) are smooth)
    */
    double s_star() const {
      return problem->s_star();
    }
    
    /*!
      estimate the compression constants alpha_k in
      ||A-A_k|| <= alpha_k * 2^{-s*k}
    */
    double alphak(const unsigned int k) const {
      return 2*norm_A(); // pessimistic
    }
    
    /*!
      evaluate the (unpreconditioned) right-hand side f
    */
    double f(const Index& lambda) const {
      return problem->f(lambda);
    }
    
    /*!
      approximate the wavelet coefficient set of the preconditioned right-hand side F
      within a prescribed \ell_2 error tolerance
    */
    void RHS(const double eta,
	     InfiniteVector<double, Index>& coeffs) const {
      problem->RHS(eta, coeffs);
    }

    /*!
      applys the galerkin system matrix corresponding to the given index set
      'window' to vector x. Missing entries will be computed
    */
    void apply(const std::set<int>& window, const Vector<double>& x,
	       Vector<double>& res) const;

    /*!
      applys the galerkin system matrix corresponding to the given index set
      'window' to vector x. Missing entries will be computed
    */
    bool CG(const std::set<int>& window, const Vector<double> &b, Vector<double> &xk,
	    const double tol, const unsigned int maxiter, unsigned int& iterations);

    
    /*!
      compute (or estimate) ||F||_2
    */
    double F_norm() const { return problem->F_norm(); }
    
    /*!
      w += factor * (stiffness matrix entries in column lambda on level j, p)
    */
    void add_level (const Index& lambda,
		    //InfiniteVector<double, Index>& w,
		    Vector<double>& w,
		    const int j,
		    const double factor,
		    const int J,
		    const CompressionStrategy strategy = St04a,
                    const int jmax = 99,
                    const int pmax = 0,
                    const double a = 0,
                    const double b = 0) const;
    
    
    void set_normA(const double norm_A_new);
    void set_normAinv(const double norm_Ainv_new);

    /*!
      clear entries cache for A
    */
    void clear_cache() {
      entries_cache.clear();
    }
    
  protected:
    //! the underlying (uncached) problem
    const PROBLEM* problem;
   
    // type of one block in one column of stiffness matrix  A
    typedef std::map<int, double> Block;
    
    // type of one column in the entry cache of A
    // the key codes the level, that data are the entries
    typedef std::map<int, Block> Column;
    
    // type of the entry cache of A
    //typedef std::map<Index, Column> ColumnCache;
    typedef std::map<int, Column> ColumnCache;

    // entries cache for A (mutable to overcome the constness of add_column())
    mutable ColumnCache entries_cache;
    
    // estimates for ||A|| and ||A^{-1}||
    mutable double normA, normAinv;
  };

  /*!
    This class provides a cache layer for generic (preconditioned, cf. precond.h)
    infinite-dimensional matrix problems of the form
    
      Au = P^{-1}LQ^{-1}u = P^{-1}F.

    The operator equation is not assumed to be induced by a local operator,
    i.e., the cache class should also work in the case of integral operators.
    All evaluations of the bilinear form a(.,.) are cached.
    Internally, the cache is managed as follows. The nonzero values of the bilinear
    form a(.,.) are stored in a map.

    The template class CachedProblem implements the minimal signature to be
    used within the APPLY routine.
  */
  template <class PROBLEM>
  class CachedProblemLocal
//     : public FullyDiagonalDyadicPreconditioner<typename PROBLEM::Index>
    : public FullyDiagonalEnergyNormPreconditioner<typename PROBLEM::Index>
  {
  public:
    /*!
      constructor from an uncached problem,
      you can specify the estimates for ||A|| and ||A^{-1}||
      (if zero, CachedProblem will compute the estimates)
    */
    CachedProblemLocal(const PROBLEM* P,
		       const double normA = 0.0,
		       const double normAinv = 0.0);
    
    /*!
      make wavelet basis type accessible
    */
    typedef typename PROBLEM::WaveletBasis WaveletBasis;
    
    /*!
      wavelet index class
    */
    typedef typename PROBLEM::Index Index;
    
    /*!
      read access to the basis
    */
    const WaveletBasis& basis() const { return problem->basis(); }
    
    /*!
      maximal level of the basis
    */
    int jmax() const { return problem->jmax(); }
    
    /*!
      space dimension of the problem
    */
    static const int space_dimension = PROBLEM::space_dimension;
    
    /*!
      locality of the operator
    */
    static bool local_operator() { return PROBLEM::local_operator(); }
    
    /*!
      (half) order t of the operator
    */
    double operator_order() const { return problem->operator_order(); }

    /*
     * access to the undelying problem
     */
    const PROBLEM* get_problem() const {return problem;}
    
    /*!
      evaluate the diagonal preconditioner D
    */
    double D(const Index& lambda) const {
      return problem->D(lambda);
    }
    
    /*!
      evaluate the (unpreconditioned) bilinear form a
      (cached)
    */
    double a(const Index& lambda,
	     const Index& nu) const;
    
    /*!
      estimate the spectral norm ||A||
    */
    double norm_A() const;
    
    /*!
      estimate the spectral norm ||A^{-1}||
    */
    double norm_Ainv() const;
    
    
    /*!
      Sets estimate for $\|A\|$.
    */
    void set_normA(const double _normA)
    {
      //problem->set_normA(_normA);
      normA = _normA;
    }

    /*!
      Sets estimate for $\|A^{-1}\|$.
    */
    void set_normAinv(const double nAinv)
    {
      //problem->set_normAinv(nAinv);
      normAinv = nAinv;
    }
    
    
    
    /*!
      estimate compressibility exponent s^*
      (we assume that the coefficients a(x),q(x) are smooth)
    */
    double s_star() const {
      return problem->s_star();
    }
    
    /*!
      estimate the compression constants alpha_k in
      ||A-A_k|| <= alpha_k * 2^{-s*k}
    */
    double alphak(const unsigned int k) const {
      return 2*norm_A(); // pessimistic
    }
    
    /*!
      evaluate the (unpreconditioned) right-hand side f
    */
    double f(const Index& lambda) const {
      return problem->f(lambda);
    }
    
    /*!
      approximate the wavelet coefficient set of the preconditioned right-hand side F
      within a prescribed \ell_2 error tolerance
    */
    void RHS(const double eta,
	     InfiniteVector<double, Index>& coeffs) const {
      problem->RHS(eta, coeffs);
    }

    /*!
      approximate the wavelet coefficient set of the preconditioned right-hand side F
      restricted to patch p within a prescribed \ell_2 error tolerance
    */
    void RHS(const double eta,
	     const int p,
	     InfiniteVector<double, Index>& coeffs) const {
      problem->RHS(eta, p, coeffs);
    }

    /*!
      applys the galerkin system matrix corresponding to the given index set
      'window' to vector x. Missing entries will be computed
    */
    void apply(const std::set<int>& window, const Vector<double>& x,
	       Vector<double>& res) const;

    /*!
      applys the galerkin system matrix corresponding to the given index set
      'window' to vector x. Missing entries will be computed
    */
    bool CG(const std::set<int>& window, const Vector<double> &b, Vector<double> &xk,
	    const double tol, const unsigned int maxiter, unsigned int& iterations);

    
    /*!
      compute (or estimate) ||F||_2
    */
    double F_norm() const { return problem->F_norm(); }

    /*!
      compute (or estimate) ||F^(i)||_2
    */
    double F_norm_local(const int patch) const { return problem->F_norm_local(patch); }
    
    /*!
      w += factor * (stiffness matrix entries in column lambda on level j restricted to the rows belonging to patch p)
    */
    void add_level (const Index& lambda,
		    const int p,
		    Vector<double>& w,
		    const int j,
		    const double factor,
		    const int J,
		    const CompressionStrategy strategy = St04a) const;
    
    
    /*!
      clear entries cache for A
    */
    void clear_cache() {
      for (int i = 0; i < ( problem->frame().n_p() ); ++i)
      {
        entries_cache[i].clear();
      }
    }

    //! just for Tests on L-domain. min. and max. eigenvalues of local stiffness matrix for each patch
    double c1_patch0;
    double c2_patch0;
    double c1_patch1;
    double c2_patch1;
    
  protected:
    //! the underlying (uncached) problem
    const PROBLEM* problem;
   
    // type of one block in one column of stiffness matrix  A
    typedef std::map<int, double> Block;
    
    // type of one column in the entry cache of A
    // the key codes the level, that data are the entries
    typedef std::map<int, Block> Column;
    
    // type of the entry cache of A
    //typedef std::map<Index, Column> ColumnCache;
    typedef std::map<int, Column> ColumnCache;

    // entries cache for A (mutable to overcome the constness of add_column())
    //mutable Array1D<ColumnCache> entries_cache;
    mutable Array1D<ColumnCache> entries_cache;

    // estimates for ||A|| and ||A^{-1}||
    mutable double normA, normAinv;
  };


  /*!
    cached problem which uses a precomputed SparseMatrix from a file
  */
  template <class PROBLEM>
  class CachedProblemFromFile
    : public FullyDiagonalEnergyNormPreconditioner<typename PROBLEM::Index>
  {
  public:
    /*!
      constructor from an uncached problem and a filename,
      you can specify the estimates for ||A|| and ||A^{-1}||
      (if zero, CachedProblem will compute the estimates)
    */
    CachedProblemFromFile(const PROBLEM* P,
			  const char* filename,
			  const int jmax,
			  const double normA = 0.0,
			  const double normAinv = 0.0);
    
    /*!
      make wavelet basis type accessible
    */
    typedef typename PROBLEM::WaveletBasis WaveletBasis;
    
    /*!
      wavelet index class
    */
    typedef typename PROBLEM::Index Index;
    
    /*!
      read access to the basis
    */
    const WaveletBasis& basis() const { return problem->basis(); }
    
    /*!
      space dimension of the problem
    */
    static const int space_dimension = PROBLEM::space_dimension;
    
    /*!
      locality of the operator
    */
    static bool local_operator() { return PROBLEM::local_operator(); }
    
    /*!
      (half) order t of the operator
    */
    double operator_order() const { return problem->operator_order(); }
    
    /*!
      evaluate the diagonal preconditioner D
    */
    double D(const Index& lambda) const {
      return problem->D(lambda);
    }
    
    /*!
      evaluate the (unpreconditioned) bilinear form a
      (cached)
    */
    double a(const Index& lambda,
	     const Index& nu) const;
    
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
      (we assume that the coefficients a(x),q(x) are smooth)
    */
    double s_star() const {
      return problem->s_star();
    }
    
    /*!
      estimate the compression constants alpha_k in
      ||A-A_k|| <= alpha_k * 2^{-s*k}
    */
    double alphak(const unsigned int k) const {
      return 2*norm_A(); // pessimistic
    }
    
    /*!
      evaluate the (unpreconditioned) right-hand side f
    */
    double f(const Index& lambda) const {
      return problem->f(lambda);
    }
    
    /*!
      approximate the wavelet coefficient set of the preconditioned right-hand side F
      within a prescribed \ell_2 error tolerance
    */
    void RHS(const double eta,
	     InfiniteVector<double, Index>& coeffs) const {
      problem->RHS(eta, coeffs);
    }
    
    /*!
      compute (or estimate) ||F||_2
    */
    double F_norm() const { return problem->F_norm(); }
    
    /*!
      w += factor * (stiffness matrix entries in column lambda on level j)
    */
    void add_level (const Index& lambda,
		    //InfiniteVector<double, Index>& w,
		    Vector<double>& w,
		    const int j,
		    const double factor,
		    const int J,
		    const CompressionStrategy strategy = St04a) const;
    
  protected:
    //! the underlying (uncached) problem
    const PROBLEM* problem;
   
    //! maximal level
    const int jmax_;

    // entries cache for A (mutable to overcome the constness of add_column())
    mutable SparseMatrix<double> entries_cache;
    
    // estimates for ||A|| and ||A^{-1}||
    mutable double normA, normAinv;
  };

}

#include <galerkin/cached_problem.cpp>

#endif
