// implementation of LaurentPolynomial inline functions

#include <cassert>
#include <algorithm>

namespace MathTL
{
  template <class R>
  LaurentPolynomial<R>::LaurentPolynomial()
    : InfiniteVector<R,int>(), Function<1,R>()
  {
  }

  template <class R>
  LaurentPolynomial<R>::LaurentPolynomial(const LaurentPolynomial<R>& p)
    : InfiniteVector<R,int>(p)
  {
  }

  template <class R>
  LaurentPolynomial<R>::LaurentPolynomial(const R value)
    : InfiniteVector<R,int>(), Function<1,R>()
  {
    set_coefficient(0, value);
  }

  template <class R>
  LaurentPolynomial<R>::~LaurentPolynomial()
  {
  }

  template <class R>
  LaurentPolynomial<R>& LaurentPolynomial<R>::operator = (const LaurentPolynomial<R>& p)
  {
    InfiniteVector<R,int>::operator = (p);
    return *this;
  }

  template <class R>
  inline
  R LaurentPolynomial<R>::get_coefficient(const int k) const
  {
    return InfiniteVector<R,int>::operator [] (k);
  }

  template <class R>
  inline
  void LaurentPolynomial<R>::set_coefficient(const int k,
					     const R coeff)
  {
    InfiniteVector<R,int>::operator [] (k) = coeff;
  }

  template <class R>
  inline
  typename LaurentPolynomial<R>::const_iterator
  LaurentPolynomial<R>::begin() const
  {
    return InfiniteVector<R,int>::begin();
  }

  template <class R>
  inline
  typename LaurentPolynomial<R>::const_iterator
  LaurentPolynomial<R>::end() const
  {
    return InfiniteVector<R,int>::end();
  }  

  template <class R>
  inline
  typename LaurentPolynomial<R>::const_reverse_iterator
  LaurentPolynomial<R>::rbegin() const
  {
    return InfiniteVector<R,int>::rbegin();
  }
  
  template <class R>
  inline
  typename LaurentPolynomial<R>::const_reverse_iterator
  LaurentPolynomial<R>::rend() const
  {
    return InfiniteVector<R,int>::rend();
  }

  template <class R>
  R LaurentPolynomial<R>::value(const R x) const
  {
    assert(x != 0 || get_coefficient(0) == R(0));

    R r(0);
    
    // Horner scheme for a shifted Laurent polynomial
    if (!empty())
      r = *rbegin();
    for (const_reverse_iterator it(rbegin()); it != rend();)
      {
 	for (int i((it++).index()); i > it.index(); i--)
 	  r *= x;
 	r += *it;
      }

     // shift back
    for (int i(begin().index()); i < 0; i++)
      r /= x;

    return r;
  }

  template <class R>
  inline
  R LaurentPolynomial<R>::value(const Point<1>& p,
				const unsigned int component) const
  {
    return value(p[0]);
  }

  template <class R>
  inline
  void LaurentPolynomial<R>::vector_value(const Point<1> &p,
					  Vector<R>& values) const
  {
    values.resize(1, false);
    values[0] = value(p[0]);
  }

  template <class R>
  inline
  void LaurentPolynomial<R>::add(const LaurentPolynomial<R>& p)
  {
    InfiniteVector<R,int>::add(p);
  }

  template <class R>
  inline
  void LaurentPolynomial<R>::add(const R s, const LaurentPolynomial<R>& p)
  {
    InfiniteVector<R,int>::add(s, p);
  }

  template <class R>
  inline
  void LaurentPolynomial<R>::sadd(const R s, const LaurentPolynomial<R>& p)
  {
    InfiniteVector<R,int>::sadd(s, p);
  }

  template <class R>
  inline
  LaurentPolynomial<R>&
  LaurentPolynomial<R>::operator += (const LaurentPolynomial<R>& p)
  {
    add(p);
    return *this;
  }

  template <class R>
  inline
  LaurentPolynomial<R>
  LaurentPolynomial<R>::operator + (const LaurentPolynomial<R>& p) const
  {
    return (LaurentPolynomial<R>(*this) += p);
  }

  template <class R>
  inline
  void LaurentPolynomial<R>::subtract(const LaurentPolynomial<R>& p)
  {
    InfiniteVector<R,int>::subtract(p);
  }

  template <class R>
  inline
  LaurentPolynomial<R>&
  LaurentPolynomial<R>::operator -= (const LaurentPolynomial<R>& p)
  {
    subtract(p);
    return *this;
  }

  template <class R>
  inline
  LaurentPolynomial<R> LaurentPolynomial<R>::operator - () const
  {
    return (LaurentPolynomial<R>() -= *this);
  }

  template <class R>
  inline
  LaurentPolynomial<R>
  LaurentPolynomial<R>::operator - (const LaurentPolynomial<R>& p) const
  {
    return (LaurentPolynomial<R>(*this) -= p);
  }

  template <class R>
  inline
  LaurentPolynomial<R>&
  LaurentPolynomial<R>::operator *= (const R c)
  {
    InfiniteVector<R,int>::scale(c);
    return *this;
  }

  template <class R>
  inline
  LaurentPolynomial<R>
  LaurentPolynomial<R>::operator * (const R c) const
  {
    return (LaurentPolynomial<R>(*this) *= c);
  }

  template <class R>
  void LaurentPolynomial<R>::multiply(const LaurentPolynomial<R>& p)
  {
    InfiniteVector<R,int> coeffs;
    for (const_iterator it(begin()), itend(end()); it != itend; ++it)
      for (const_iterator itp(p.begin()), itpend(p.end()); itp != itpend; ++itp)
	coeffs[it.index()+itp.index()] += *it * *itp;

    swap(coeffs);    
  }

  template <class R>
  inline
  LaurentPolynomial<R>&
  LaurentPolynomial<R>::operator *= (const LaurentPolynomial<R>& p)
  {
    multiply(p);
    return *this;
  }

  template <class R>
  inline
  LaurentPolynomial<R>
  LaurentPolynomial<R>::operator * (const LaurentPolynomial<R>& p)
  {
    return (LaurentPolynomial(*this) *= p);
  }

  template <class R>
  LaurentPolynomial<R>
  LaurentPolynomial<R>::power(const unsigned int k) const
  {
    LaurentPolynomial<R> r(1);
    
    for (unsigned int i(0); i < k; i++)
      r.multiply(*this);
    
    return r;
  }

  template <class R>
  std::ostream& operator << (std::ostream& s, const LaurentPolynomial<R>& p)
  {
    for (typename LaurentPolynomial<R>::const_iterator it(p.begin());
 	 it != p.end(); ++it)
      {
 	if (it != p.begin())
 	  s << "+";
 	s << "(" << *it << ")";
 	if (it.index() != 0)
 	  {
 	    s << "z";
 	    if (it.index() != 1)
 	      {
 		s << "^";
 		if (it.index() < 0)
 		  s << "{" << it.index() << "}";
 		else
 		  s << it.index();
 	      }
 	  }
      }
    
    return s;
  }
}
