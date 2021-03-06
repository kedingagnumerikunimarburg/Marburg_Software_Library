// implementation of MathTL::Vector inline functions ...

#include <io/vector_io.h>
#include <cassert>
#include <algorithm>
#include <functional>
#include <sstream>
#include <cstring>

namespace MathTL
{
  template <class C>
  inline
  Vector<C>::Vector()
    : values_(0), size_(0)
  {
  }
  
  template <class C>
  inline
  Vector<C>::Vector(const size_type s, const bool initialize)
    : size_(s)
  {
    if (s == 0)
      values_ = 0;
    else
      {
	values_ = new C[s]; // calls C()
	
	if (initialize)
	  (*this).operator = (0);
      }
  }

  template <class C>
  Vector<C>::Vector(const size_type s, const char* str)
    : values_(0), size_(0)
  {
    resize(s, false);

    std::istringstream ins(str);
    for (size_type i(0); i < s && ins.good(); i++)
      ins >> (*this).operator () (i);
  }

  template <class C>
  inline
  Vector<C>::Vector(const Vector<C>& v)
    : size_(v.size())
  {
    if (size_ == 0)
      values_ = 0;
    else
      {
	values_ = new C[size_];
	std::copy(v.begin(), v.end(), begin());
      }
  }
 
  template <class C>
  inline
  Vector<C>::~Vector()
  {
    if (values_ != 0)
      {
	delete [] values_;
 	values_ = 0;
 	size_ = 0;
      }
  }
  
  template <class C>
  inline
  typename Vector<C>::size_type
  Vector<C>::size() const
  {
    return size_;
  }

  template <class C>
  inline
  typename Vector<C>::size_type
  Vector<C>::memory_consumption() const
  {
    return sizeof(*this) + size_*sizeof(C);
  }

  template <class C>
  inline
  bool Vector<C>::empty() const
  {
    return size()==0;
  }

  template <class C>
  void Vector<C>::resize(const size_type s, const bool initialize)
  {
    if (s == 0)
      {
	if (values_ != 0)
	  {
	    delete [] values_;
	    values_ = 0;
	  }
	size_ = 0;
      }
    else
      {
	if (size_ != s)
	  {
	    if (values_ != 0) delete [] values_;
	    values_ = new C[s];
	    size_ = s;
	  }

	if (initialize)
	  (*this).operator = (0);
      }
  }

  template <class C>
  inline
  const C Vector<C>::operator [] (const size_type i) const
  {
    assert(i < size_);
    return values_[i];
  }

  template <class C>
  inline
  const C Vector<C>::operator () (const size_type i) const
  {
    assert(i < size_);
    return values_[i];
  }

  template <class C>
  inline
  C& Vector<C>::operator [] (const size_type i)
  {
    assert(i < size_);
    return values_[i];
  }

  template <class C>
  inline
  C& Vector<C>::operator () (const size_type i)
  {
    assert(i < size_);
    return values_[i];
  }

  template <class C>
  inline
  typename Vector<C>::const_iterator
  Vector<C>::begin() const
  {
    return &values_[0];
  }

  template <class C>
  inline
  typename Vector<C>::iterator
  Vector<C>::begin()
  {
    return &values_[0];
  }

  template <class C>
  inline
  typename Vector<C>::const_iterator
  Vector<C>::end() const
  {
    return &values_[size_];
  }

  template <class C>
  inline
  typename Vector<C>::iterator
  Vector<C>::end()
  {
    return &values_[size_];
  }

  template <class C>
  inline
  Vector<C>& Vector<C>::operator = (const C c)
  {
    assert(size() > 0);

    std::fill(begin(), end(), c);

    return *this;
  }

  template <class C>
  Vector<C>& Vector<C>::operator = (const Vector<C>& v)
  {
    resize(v.size(), false);
    std::copy(v.begin(), v.end(), begin());

    return *this;
  }

  template <class C>
  inline
  void Vector<C>::swap(Vector<C>& v)
  {
    std::swap(size_, v.size_);
    std::swap(values_, v.values_);
  }

  template <class C>
  void Vector<C>::compress(const double eta)
  {
    for (iterator it(begin()), itend(end());
	 it != itend; ++it)
      {
	if (fabs(*it) < eta)
	  *it = C(0);
      }
  }
  
  template <class C>
  void Vector<C>::shrinkage(const double mu)
  {
    for (iterator it(begin()), itend(end());
	 it != itend; ++it)
      {
	if (fabs(*it) <= mu)
	  *it = C(0);
        if(*it > mu)
            *it-=mu*0.5;
        if(*it < -mu)
            *it+=mu*0.5;
      }
  }
  

  template <class C>
  double
  Vector<C>::wrmsqr_norm(const double atol, const double rtol,
			 const Vector<C>& v, const Vector<C>& w) const
  {
    assert(size() == w.size());

    double result = 0;
    
    for (const_iterator it(begin()), itv (v.begin()), itw(w.begin()), itend(end());
	 it != itend; ++it, ++itv, ++itw)
      {
	const double help = *it / (atol + rtol * std::max(*itv, *itw));
	result += help * help;
      }

    return result == 0 ? 0 : sqrt(result/size());
  }
  
  
  template <class C>
  void Vector<C>::matlab_output (const char *file, const char *Vectorname) const
  {
    
	char Filename[200];
	Filename[0] = '\x0';
	
	strcat(Filename, file);
	strcat(Filename, ".m");
	
	std::ofstream m_file(Filename);
	
	m_file << Vectorname << "=load('" << file << ".dat');" << std::endl;
	
	m_file.close();
	
	Filename[0] = '\x0';
	
	strcat(Filename, file);
	strcat(Filename, ".dat");

	std::ofstream dat_file(Filename);
	
	dat_file.setf(std::ios::scientific, std::ios::fixed);
	dat_file.precision(15);
	
 	for (typename Vector<C>::size_type i(0); i < size(); ++i)
 	      dat_file << this->operator () (i) << " ";
 	    dat_file << std::endl;
 	  
	
	dat_file.close();
      
  }
  
  
  
  template <class C>
  template <class C2>
  bool Vector<C>::operator == (const Vector<C2>& v) const
  {
    if (size_ != v.size()) return false;
    return std::equal(begin(), end(), v.begin());
  }

  template <class C>
  template <class C2>
  inline
  bool Vector<C>::operator != (const Vector<C2>& v) const
  {
    return !((*this) == v);
  }

  template <class C>
  template <class C2>
  inline
  bool Vector<C>::operator < (const Vector<C2>& v) const
  {
    assert(size_ == v.size());

    return std::lexicographical_compare(begin(), end(), v.begin(), v.end());
  }

  template <class C>
  template <class C2>
  void Vector<C>::add(const Vector<C2>& v)
  {
    assert(size_ > 0);
    assert(size_ == v.size());

    iterator it(begin()), itend(end());
    typename Vector<C2>::const_iterator itv(v.begin());
    while (it != itend)
      *it++ += *itv++;
  }
   
  template <class C>
  template <class C2>
  void Vector<C>::add(const C2 s, const Vector<C2>& v)
  {
    assert(size_ > 0);
    assert(size_ == v.size());

    iterator it(begin()), itend(end());
    typename Vector<C2>::const_iterator itv(v.begin());
    while (it != itend)
      *it++ += s * *itv++;
  }
   
  template <class C>
  template <class C2>
  void Vector<C>::sadd(const C s, const Vector<C2>& v)
  {
    assert(size_ > 0);
    assert(size_ == v.size());

    iterator it(begin()), itend(end());
    typename Vector<C2>::const_iterator itv(v.begin());
    while(it != itend)
      {
	*it = s*(*it) + *itv++;
	++it;
      }
  }
  
  template <class C>
  void Vector<C>::scale(const C s)
  {
    assert(size_ > 0);
    
    iterator it(begin()), itend(end());
    while(it != itend)
      *it++ *= s;
  }

  template <class C>
  template <class C2>
  inline
  Vector<C>& Vector<C>::operator += (const Vector<C2>& v)
  {
    add(v); // handles the assertions
    return *this;
  }

  template <class C>
  template <class C2>
  void Vector<C>::subtract(const Vector<C2>& v)
  {
    assert(size_ > 0);
    assert(size_ == v.size());

    iterator it(begin()), itend(end());
    typename Vector<C2>::const_iterator itv(v.begin());
    while (it != itend)
      *it++ -= *itv++;
  }

  template <class C>
  template <class C2>
  inline
  Vector<C>& Vector<C>::operator -= (const Vector<C2>& v)
  {
    subtract(v); // handles the assertions
    return *this;
  }
   
  template <class C>
  Vector<C>& Vector<C>::operator *= (const C s)
  {
    scale(s); // handles the assertions
    return *this;
  }

  template <class C>
  Vector<C>& Vector<C>::operator /= (const C s)
  {
    // we don't catch the division by zero exception here!
    return (*this *= 1.0/s);
  }

  template <class C>
  template <class C2>
  const C Vector<C>::inner_product (const Vector<C2>& v) const
  {
    assert(size_ == v.size());

    if (this == reinterpret_cast<const Vector<C>*>(&v))
      return l2_norm_sqr(*this);

    C r(0);
    
    const_iterator it(begin()), itend(end());
    typename Vector<C2>::const_iterator itv(v.begin());
    while(it != itend)
      r += *it++ * *itv++;

    return r;
  }

  template <class C>
  template <class C2>
  const C Vector<C>::operator * (const Vector<C2>& v) const
  {
    return inner_product(v); // handles the assertions
  }

  template <class C>
  inline
  void swap(Vector<C>& v1, Vector<C>& v2)
  {
    v1.swap(v2);
  }

  template <class C, class C2>
  Vector<C> operator + (const Vector<C>& v1, const Vector<C2>& v2)
  {
    Vector<C> r(v1);
    r += v2;
    return r;
  }

  template <class C, class C2>
  Vector<C> operator - (const Vector<C>& v1, const Vector<C2>& v2)
  {
    Vector<C> r(v1);
    r -= v2;
    return r;
  }

  template <class C>
  inline
  std::ostream& operator << (std::ostream& os, const Vector<C>& v)
  {
    print_vector(v, os);
    return os;
  }
}
