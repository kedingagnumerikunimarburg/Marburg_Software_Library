// -*- c++ -*-

// +--------------------------------------------------------------------+
// | This file is part of WaveletTL - the Wavelet Template Library      |
// |                                                                    |
// | Copyright (c) 2002-2006                                            |
// | Thorsten Raasch, Manuel Werner                                     |
// +--------------------------------------------------------------------+

#ifndef _WAVELETTL_LDOMAIN_INDEX_H
#define _WAVELETTL_LDOMAIN_INDEX_H

#include <iostream>
using std::cout;
using std::endl;

#include <utils/multiindex.h>

using MathTL::MultiIndex;

namespace WaveletTL
{
  template <class IBASIS> class LDomainBasis;
  
  /*!
    An index class for wavelet bases of the L-shaped domain in R^2
  */
  template <class IBASIS>
  class LDomainIndex
  {
  public:
    //! type index type
    typedef MultiIndex<int,2> type_type;
    
    //! translation index type
    typedef MultiIndex<int,2> translation_type;

    /*!
      constructor with a given L-domain basis
      (also serves as a default constructor, but yields an invalid index
      in this case, because the underlying bases must be specified to work correctly)
    */
    LDomainIndex(const LDomainBasis<IBASIS>* basis = 0);

    //! preincrement
    LDomainIndex& operator ++ ();

    //! scale j
    const int j() const { return j_; }

    //! type e
    const type_type& e() const { return e_; }

    //! patch p
    const int p() const { return p_; }

    //! translation index k
    const translation_type& k() const { return k_; }

  protected:
    //! pointer to the underlying basis
    const LDomainBasis<IBASIS>* basis_;

    //! scale
    int j_;
    
    //! type
    MultiIndex<int,2> e_;

    //! patch
    int p_;
    
    //! translation
    MultiIndex<int,2> k_;

  };

  //! stream output
  template <class IBASIS>
  inline std::ostream& operator << (std::ostream& os,
				    const LDomainIndex<IBASIS>& lambda)
  {
    using namespace std;
    os << "("
       << lambda.j()
       << ","
       << lambda.e()
       << ","
       << lambda.p()
       << ","
       << lambda.k()
       << ")";
    return os;
  }


}

#include <Ldomain/ldomain_index.cpp>

#endif
