// -*- c++ -*-

// +--------------------------------------------------------------------+
// | This file is part of WaveletTL - the Wavelet Template Library      |
// |                                                                    |
// | Copyright (c) 2002-2009                                            |
// | Thorsten Raasch, Manuel Werner                                     |
// +--------------------------------------------------------------------+

#ifndef _WAVELETTL_CUBE_INDEX_H
#define _WAVELETTL_CUBE_INDEX_H

#include <iostream>
using std::cout;
using std::endl;

#include <utils/multiindex.h>

using MathTL::MultiIndex;

namespace WaveletTL
{
  template <class IBASIS, unsigned int DIM> class CubeBasis;
  
  /*!
    An index class for tensor product wavelet bases over the d-dimensional
    unit cube [0,1]^d (or mapped versions thereof).
  */
  template <class IBASIS, unsigned int DIM, class CUBEBASIS = CubeBasis<IBASIS,DIM> >
  class CubeIndex
  {
  public:
    //! type index type
    typedef MultiIndex<int,DIM> type_type;
    
    //! translation index type
    typedef MultiIndex<int,DIM> translation_type;

    /*!
      constructor with a given cube basis
      (also serves as a default constructor, but yields an invalid index
      in this case, because the underlying bases must be specified to work correctly)
    */
    CubeIndex(const CUBEBASIS* basis = 0);

    /*!
      constructor with given j,e,k
    */
    CubeIndex(const int j,
	      const type_type& e,
	      const translation_type& k,
	      const CUBEBASIS* basis);

    //! copy constructor
    CubeIndex(const CubeIndex& lambda);

    //! copy index from const pointer
    CubeIndex(const CubeIndex* lambda);


    /*!
      constructor.
      We always assume to have a lexicographical ordering for the elements in the wavelet bases, w.r.t.
      the tuple ( j , e , k ).
      According to this ordering there exists a mapping from the totality of wavelet indices into
      the nonnegative integers. This routine creates an index from the given number corresponding
      to this ordering.
      This constructor cannot be used for generators on levels j > j0.
    */
    CubeIndex(const int number,
	      const CUBEBASIS* basis);

    //! assignment
    CubeIndex& operator = (const CubeIndex& lambda);

    //! check equality
    bool operator == (const CubeIndex& lambda) const;
    
    //! check non-equality
    inline bool operator != (const CubeIndex& lambda) const
    { return !(*this == lambda); }
    
    //! preincrement
    CubeIndex& operator ++ ();

    //! lexicographic order <
    bool operator < (const CubeIndex& lambda) const;

    //! lexicographic order <=
    bool operator <= (const CubeIndex& lambda) const
    { return (*this < lambda || *this == lambda); }

    //! scale j
    const int j() const { return j_; }

    //! type e
    const type_type& e() const { return e_; }

    //! translation index k
    const translation_type& k() const { return k_; }

    //! underlying basis
    const CUBEBASIS* basis() const { return basis_; }

    const int number() const { return num_; }

    /*!
      inverse of constructor
      'CubeIndex(const unsigned int number,
                 const CUBEBASIS* basis)'
    */    
    void set_number();

    /*
     get the level, type and translations parameter k frome the number and the basis
    */
    void get_Index_Parameter(const CUBEBASIS* basis, const int num, int& j, type_type& e, translation_type& k) const;

  protected:
    
    //! pointer to the underlying basis
    const CUBEBASIS* basis_;

    //! scale
    int j_;
    
    //! type
    MultiIndex<int,DIM> e_;

    //! translation
    MultiIndex<int,DIM> k_;

    //! number of the index, only for the elements of a wavelet bases
    int num_;

  };

  //! stream output
  template <class IBASIS, unsigned int DIM, class CUBEBASIS>
  inline std::ostream& operator << (std::ostream& os,
				    const CubeIndex<IBASIS,DIM,CUBEBASIS>& lambda)
  {
    using namespace std;
    os << "("
       << lambda.j()
       << ","
       << lambda.e()
       << ","
       << lambda.k()
       << ")";
    return os;
  }

  /*!
    index of first generator on level j >= j0
  */
  template <class IBASIS, unsigned int DIM, class CUBEBASIS>
  CubeIndex<IBASIS,DIM,CUBEBASIS>
  first_generator(const CUBEBASIS* basis, const int j);

  /*!
    index of last generator on level j >= j0
  */
  template <class IBASIS, unsigned int DIM, class CUBEBASIS>
  CubeIndex<IBASIS,DIM,CUBEBASIS>
  last_generator(const CUBEBASIS* basis, const int j);

  /*!
    index of first wavelet on level j >= j0
  */
  template <class IBASIS, unsigned int DIM, class CUBEBASIS>
  CubeIndex<IBASIS,DIM,CUBEBASIS>
  first_wavelet(const CUBEBASIS* basis, const int j);
  
  /*!
    index of last wavelet on level j >= j0
  */
  template <class IBASIS, unsigned int DIM, class CUBEBASIS>
  CubeIndex<IBASIS,DIM,CUBEBASIS>
  last_wavelet(const CUBEBASIS* basis, const int j);

  /*!
    number of first generator on level j0
  */
  template <class IBASIS, unsigned int DIM, class CUBEBASIS>
  const int
  first_generator_num(const CUBEBASIS* basis);

  /*!
    number of last generator on level j0
  */
  template <class IBASIS, unsigned int DIM, class CUBEBASIS>
  const int
  last_generator_num(const CUBEBASIS* basis);

  /*!
    number of first wavelet on level j >= j0
  */
  template <class IBASIS, unsigned int DIM, class CUBEBASIS>
  const int
  first_wavelet_num(const CUBEBASIS* basis, const int j);
  
  /*!
    number of last wavelet on level j >= j0
  */
  template <class IBASIS, unsigned int DIM, class CUBEBASIS>
  const int
  last_wavelet_num(const CUBEBASIS* basis, const int j);

}

#include <cube/cube_index.cpp>

#endif
