// -*- c++ -*-

// +--------------------------------------------------------------------+
// | This file is part of MathTL - the Mathematical Template Library    |
// |                                                                    |
// | Copyright (c) 2002-2004                                            |
// | Thorsten Raasch                                                    |
// +--------------------------------------------------------------------+

#ifndef _MATHTL_INFINITE_VECTOR_H
#define _MATHTL_INFINITE_VECTOR_H

#include <cmath>
#include <map>
#include <vector>
#include <algorithm>
// #include "map_iterator.h"

namespace MathTL
{
  /*!
    A model class InfiniteVector<C,I> for inherently sparse,
    arbitrarily indexed vectors
      x = (x_i)_{i\in I}
    with entries from a (scalar) class C.
    InfiniteVector<C,I> may act as a realization of coefficient_type in the FWT.
    
    Although internally, we will model InfiniteVector as a map<I,C>,
    the access to the vector entries takes place by a nested iterator
    class (compare the deal.II matrix classes) which is STL-compatible.
    
  */
  template <class C, class I>
  class InfiniteVector
    : protected std::map<I,C>
  {
  public:
    /*!
      default constructor: yields empty (zero) vector
    */
    InfiniteVector();

    /*!
      copy constructor
    */
    InfiniteVector(const InfiniteVector<C,I>& v);

    /*!
      forward declaration of const_iterator
    */
    class const_iterator;
    
    /*!
      nested accessor class for const_iterator
    */
    class Accessor
    {
    public:
      /*!
	construct an accessor from a map::const_iterator
       */
      Accessor(const typename std::map<I,C>::const_iterator& entry);

      /*!
	index of current entry
      */
      I index() const;

      /*!
	value of current vector entry
      */
      C value() const;

      /*!
	compare two accessors
      */
      bool operator == (const Accessor& a) const;

      /*!
	non-equality test
      */
      bool operator != (const Accessor& a) const;

    protected:
      /*!
	STL iterator pointing to current entry
	(one would also like Accessor to be a direct descendant
	of std::map<I,C>::const_iterator, however this posed
	some problems implementing it, maybe we do so
	in a later version)
      */
      typename std::map<I,C>::const_iterator entry_;

      /*!
	const_iterator should have access to protected members
       */
      friend class const_iterator;
    };

    /*!
      STL-compliant const_iterator scanning the nontrivial entries
    */
    class const_iterator
    {
    public:
      /*!
	constructs a const_iterator from a map::const_iterator
      */
      const_iterator(const typename std::map<I,C>::const_iterator& entry);

      /*!
	prefix increment of the const_iterator
      */
      const_iterator& operator ++ ();

      /*!
	dereference const_iterator
      */
      const Accessor& operator * () const;

      /*!
	dereference const_iterator
      */
      const Accessor* operator -> () const;

      /*!
	compare positions of two iterators
      */
      bool operator == (const const_iterator& it) const;

      /*!
	non-equality test
      */
      bool operator != (const const_iterator& it) const;

      /*!
	comparison, corresponds to the order relation on I
       */
      bool operator < (const const_iterator& it) const;

    private:
      /*!
	store accessor to current vector entry
      */
      Accessor accessor_;
    };

    /*!
      const_iterator pointing to the first nontrivial vector entry
    */
    const_iterator begin() const;

    /*!
      const_iterator pointing to one after the last nontrivial vector entry
    */
    const_iterator end() const;

    /*!
      equality test
    */
    bool operator == (const InfiniteVector<C,I>& v) const;

    /*!
      non-equality test
    */
    bool operator != (const InfiniteVector<C,I>& v) const;

    /*!
      read-only access to the vector entries
    */
    C operator [] (const I& index) const;

    /*!
      read-write access to the vector entries
    */
    C& operator [] (const I& index);

  };


  //! helper struct modelling a decreasing order in modulus
  /*!
    helper struct to handle decreasing order in modulus
    for pairs, with respect to the first argument
  */
//   template <class I, class C>
//   struct decreasing_order
//     : public std::binary_function<const std::pair<I,C>&,
// 				  const std::pair<I,C>&,
// 				  bool>
//   {
//     inline bool operator () (const std::pair<I,C>& p1,
// 			     const std::pair<I,C>& p2)
//     {
//       return (fabs(p1.second) > fabs(p2.second));
//     }
//   };
  

//     //! set vector to zero
//     inline void clear() { data_.clear(); }

    
//     //! in place subtraction
//     self_type& operator -= (const self_type& a)
//     {
//       for (entry_const_iterator ita(a.begin()), itaend(a.end());
// 	   ita != itaend; ++ita)
// 	this->operator () (ita.index()) -= ita.entry();
//       return *this;
//     }
    
//     //! in place scalar multiplication
//     self_type& operator *= (const C& c)
//     {
//       for (entry_iterator it(begin()); it != end(); ++it) it.entry() *= c;
//       return *this;
//     }

//     //! in place scalar division
//     self_type& operator /= (const C& c)
//     {
//       for (entry_iterator it(begin()); it != end(); ++it) it.entry() /= c;
//       return *this;
//     }

//     //! assignment from another vector
//     self_type& operator = (const self_type& a)
//     {
//       data_.clear();
//       for (entry_const_iterator ita(a.begin()), itaend(a.end());
// 	   ita != itaend; ++ita)
// 	this->operator () (ita.index()) = ita.entry();
//       return *this;
//     }

//     //! first nontrivial entry, reading access
//     entry_const_iterator begin() const { return entry_const_iterator(data_.begin()); }

//     //! first nontrivial entry, writing access
//     entry_iterator       begin()       { return entry_iterator(data_.begin()); }

//     //! one after last nontrivial entry, reading access
//     entry_const_iterator end()   const { return entry_const_iterator(data_.end()); }

//     //! one after last nontrivial entry, writing access
//     entry_iterator       end()         { return entry_iterator(data_.end()); }

//     //! storage requirements
//     inline int size() const { return data_.size(); }

//     //! standard NCOARSE routine
//     /*!
//       NCOARSE computes optimal v such that \|*this-v\|_{\ell_2}\le\epsilon;
//       "optimal" means taking the largest entries in modulus of *this
//      */
//     void NCOARSE(const double eps, InfiniteVector<C,I>& v) const
//     {
//       // We use a straightforward implementation with complexity O(N*log(N)):
//       // - sort my entries in modulus
//       //   1. possibility: use a helper multimap object
//       //   2. possibility: use a sorted vector (preferred solution, since no
//       //                   slow insertion sort algorithm is launched!)
//       // - insert the largest in modulus entries into v until
//       //     \|*this-v\|_{\ell_2}\le\epsilon
//       //
//       // Another possibility would be binary binning, which we will implement
//       // in a later stage of the library!
//       v.clear();
//       if (size() > 0)
// 	{
// 	  // prepare vector to be sorted
// 	  std::vector<std::pair<I,C> > sv(size());
// 	  int id(0);
// 	  for (typename std::map<I,C>::const_iterator it(data_.begin()), itend(data_.end());
// 	       it != itend; ++it, ++id)
// 	    sv[id] = *it;
	  
// 	  // sort vector (Introsort, O(N*log N))
// 	  sort(sv.begin(), sv.end(), decreasing_order<I,C>());

// 	  // insert largest in modulus entries until tolerance is reached
// 	  double coarsenorm(0);
// 	  double nrm(norm(*this));
// 	  double bound(nrm*nrm - eps*eps);
// 	  typename std::vector<std::pair<I,C> >::iterator it(sv.begin());
// 	  do
// 	    {
// 	      coarsenorm += it->second * it->second;
// 	      ++it;
// 	    }
// 	  while ((it != sv.end()) && (coarsenorm < bound));
// 	  sv.erase(it, sv.end());

// 	  // insert relevant entries in v (-> insertion sort, we hope that
// 	  // the number of entries is neglectible)
// 	  for (unsigned int i(0); i < sv.size(); i++)
// 	    v(sv[i].first) = sv[i].second;
// 	}
//     }
    
//     double weak_norm(const double tau) const
//     {
//       double r(0);
//       if (size() > 0)
// 	{
// 	  // prepare vector to be sorted
// 	  std::vector<std::pair<I,C> > sv(size());
// 	  int id(0);
// 	  for (typename std::map<I,C>::const_iterator it(data_.begin()), itend(data_.end());
// 	       it != itend; ++it, ++id)
// 	    sv[id] = *it;
	  
// 	  // sort vector (Introsort, O(N*log N))
// 	  sort(sv.begin(), sv.end(), decreasing_order<I,C>());
	  
// 	  // compute \|*this\|_{\ell^w_\tau}:=\sup_{N=1}^\infty N^{1/tau}|v_N^*|
// 	  // where the v_N^* are the decreasing rearrangement of me
// 	  for (unsigned int N(1); N <= sv.size(); N++)
// 	    r = std::max(r, pow(N, 1.0/tau) * fabs(sv[N-1].second));
// 	}
//       return r;
//     }

//   protected:
//     std::map<I,C> data_;
//   };

//   //
//   // some algorithms
  

//   //! summation
//   template <class C, class I>
//   InfiniteVector<C,I> operator + (const InfiniteVector<C,I>& v, const InfiniteVector<C,I>& w)
//   {
//     InfiniteVector<C,I> r(v);
//     r += w;
//     return r;
//   }

//   //! subtraction
//   template <class C, class I>
//   InfiniteVector<C,I> operator - (const InfiniteVector<C,I>& v, const InfiniteVector<C,I>& w)
//   {
//     InfiniteVector<C,I> r(v);
//     r -= w;
//     return r;
//   }

//   //! sign
//   template <class C, class I>
//   InfiniteVector<C,I> operator - (const InfiniteVector<C,I>& v)
//   {
//     InfiniteVector<C,I> r(v);
//     r -= C(-1);
//     return r;
//   }

//   //! scalar multiplication
//   template <class C, class I>
//   InfiniteVector<C,I> operator * (const C c, const InfiniteVector<C,I>& v)
//   {
//     InfiniteVector<C,I> r(v);
//     r *= c;
//     return r;
//   }

//   //! scalar product of two vectors
//   template <class C, class I>
//   double operator * (const InfiniteVector<C,I>& v, const InfiniteVector<C,I>& w)
//   {
//     double r(0);
//     typedef typename InfiniteVector<C,I>::entry_const_iterator entry_const_iterator;
//     entry_const_iterator itv(v.begin()), itvend(v.end()), itw(w.begin()), itwend(w.end());
//     for (; itv != itvend && itw != itwend; ++itv)
//       {
//  	while (itw != itwend && itw.index() < itv.index()) ++itw;
//  	if (itv.index() == itw.index())
//  	  r += itw.entry() * itv.entry();
//       }
//     return r;
//   }

  /*!
    stream output for infinite vectors
  */
  template<class C, class I>
  std::ostream& operator << (std::ostream& os, const InfiniteVector<C,I>& v);

//   //! ell_2 norm
//   template <class C, class I>
//   double norm(const InfiniteVector<C,I>& v)
//   {
//     double r(0);
//     typedef typename InfiniteVector<C,I>::entry_const_iterator entry_const_iterator;
//     for (entry_const_iterator it(v.begin()), itend(v.end()); it != itend; ++it)
//       r += it.entry() * it.entry();
//     return sqrt(r);
//   }
  
//   //! ell_p (quasi-) norm
//   template <class C, class I>
//   double norm(const InfiniteVector<C,I>& v, double p)
//   {
//     double r(0);
//     typedef typename InfiniteVector<C,I>::entry_const_iterator entry_const_iterator;
//     for (entry_const_iterator it(v.begin()), itend(v.end()); it != itend; ++it)
//       r += pow(fabs(it.entry()), p);
//     return pow(r, 1./p);
//   }
  
//   //! maximum norm
//   template <class C, class I>
//   double maxnorm(const InfiniteVector<C,I>& v)
//   {
//     double r(0);
//     typedef typename InfiniteVector<C,I>::entry_const_iterator entry_const_iterator;
//     for (entry_const_iterator it(v.begin()), itend(v.end()); it != itend; ++it)
//       r = std::max(fabs(it.entry()),r);
//     return r;
//   }

}

// include implementation of inline functions
#include <algebra/infinite_vector.cpp>

#endif
