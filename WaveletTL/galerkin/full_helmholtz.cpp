// implementation for full_helmholtz.h

#include <utils/map_tools.h>

using namespace MathTL;

namespace WaveletTL
{
  template <int d, int dT, int s0, int s1, int J0>
  FullHelmholtz<d,dT,s0,s1,J0>::FullHelmholtz(const SplineBasis<d,dT,P_construction,s0,s1,0,0,J0>& sb,
					      const double alpha,
					      const PreconditioningType precond)
    : G_(sb), A_(sb, no_precond), sb_(sb), alpha_(alpha), precond_(precond), j_(-1)
  {
    set_level(sb_.j0());
  }

  template <int d, int dT, int s0, int s1, int J0>
  inline
  const typename FullHelmholtz<d,dT,s0,s1,J0>::size_type
  FullHelmholtz<d,dT,s0,s1,J0>::row_dimension() const
  {
    return sb_.Deltasize(j_);
  }  
  
  template <int d, int dT, int s0, int s1, int J0>
  inline
  const typename FullHelmholtz<d,dT,s0,s1,J0>::size_type
  FullHelmholtz<d,dT,s0,s1,J0>::column_dimension() const
  {
    return row_dimension(); // square
  }  
  
  template <int d, int dT, int s0, int s1, int J0>
  void
  FullHelmholtz<d,dT,s0,s1,J0>::set_level(const int j) const
  {
    assert(j >= sb_.j0());
    if (j_ != j) {
      j_ = j;
      G_.set_level(j);
      A_.set_level(j);
      setup_D();
    }
  }

  template <int d, int dT, int s0, int s1, int J0>
  void
  FullHelmholtz<d,dT,s0,s1,J0>::set_alpha(const double alpha) const
  {
    assert(alpha >= 0);
    alpha_ = alpha;
    setup_D();
  }

  template <int d, int dT, int s0, int s1, int J0>
  void
  FullHelmholtz<d,dT,s0,s1,J0>::setup_D() const
  {
    D_.resize(sb_.Deltasize(j_));
    if (precond_ == no_precond) {
      D_ = 1.0;
    } else {
      if (precond_ == dyadic) {
	for (int k(0); k < sb_.Deltasize(sb_.j0()); k++)
	  D_[k] = alpha_ + (1<<sb_.j0());
	for (int j = sb_.j0(); j < j_; j++) {
	  for (int k(sb_.Deltasize(j)); k < sb_.Deltasize(j+1); k++)
	    D_[k] = alpha_ + (1<<j);
	}
      } else {
 	for (size_type k(0); k < D_.size(); k++)
 	  D_[k] = sqrt(diagonal(k));
      }
    }
  }
  
  template <int d, int dT, int s0, int s1, int J0>
  inline
  double
  FullHelmholtz<d,dT,s0,s1,J0>::D(const size_type k) const {
    return D_[k];
  }
  
  template <int d, int dT, int s0, int s1, int J0>
  inline
  const double
  FullHelmholtz<d,dT,s0,s1,J0>::get_entry(const size_type row, const size_type column) const
  {
    return (alpha_*G_.get_entry(row,column)+A_.get_entry(row,column))/(D(row)*D(column));
  }

  template <int d, int dT, int s0, int s1, int J0>
  inline
  const double
  FullHelmholtz<d,dT,s0,s1,J0>::diagonal(const size_type row) const
  {
    return (alpha_*G_.diagonal(row)+A_.diagonal(row));
  }
  
  template <int d, int dT, int s0, int s1, int J0>
  template <class VECTOR>
  void FullHelmholtz<d,dT,s0,s1,J0>::apply(const VECTOR& x, VECTOR& Mx,
				     const bool preconditioning) const
  {
    assert(Mx.size() == row_dimension());

    VECTOR y(x), yhelp(x.size(), false);

    if (preconditioning) {
      // apply diagonal preconditioner D^{-1}
      for (size_type k(0); k < y.size(); k++)
	y[k] /= D(k);
    }

    // apply Gramian
    G_.apply(y, yhelp);
    
    // apply unpreconditioned Laplacian
    A_.apply(y, Mx);

    // add Gramian part, y+=alpha*yhelp
    Mx.add(alpha_, yhelp);
    
    if (preconditioning) {
      // apply diagonal preconditioner D^{-1}
      for (size_type k(0); k < y.size(); k++)
	Mx[k] /= D(k);
    }
  }

  template <int d, int dT, int s0, int s1, int J0>
  void FullHelmholtz<d,dT,s0,s1,J0>::apply(const std::map<size_type,double>& x,
				     std::map<size_type,double>& Mx,
				     const bool preconditioning) const
  {
    std::map<size_type,double> y(x), yhelp;

    if (preconditioning) {
      // apply diagonal preconditioner D^{-1}
      for (std::map<size_type,double>::iterator it(y.begin()); it != y.end(); ++it)
	it->second /= D(it->first);
    }

    // apply Gramian
    G_.apply(y, yhelp);
    
    // apply unpreconditioned Laplacian
    A_.apply(y, Mx);

    // add Gramian part, Mx+=alpha*yhelp
    add_maps(yhelp, Mx, y, alpha_, 1.0);
    Mx.swap(y);

    if (preconditioning) {
      // apply diagonal preconditioner D^{-1}
      for (std::map<size_type,double>::iterator it(Mx.begin()); it != Mx.end(); ++it)
	it->second /= D(it->first);
    }
  }

  template <int d, int dT, int s0, int s1, int J0>
  void FullHelmholtz<d,dT,s0,s1,J0>::print(std::ostream &os,
				     const unsigned int tabwidth,
				     const unsigned int precision) const
  {
    if (row_dimension() == 0)
      os << "[]" << std::endl; // Matlab style
    else
      {
	unsigned int old_precision = os.precision(precision);
	for (typename FullHelmholtz<d,dT,s0,s1,J0>::size_type i(0); i < row_dimension(); ++i)
	  {
	    for (typename FullHelmholtz<d,dT,s0,s1,J0>::size_type j(0); j < column_dimension(); ++j)
	      os << std::setw(tabwidth) << std::setprecision(precision)
		 << get_entry(i, j);
	    os << std::endl;
	  }
	os.precision(old_precision);
      }
  }
  
  template <int d, int dT, int s0, int s1, int J0>
  inline
  std::ostream& operator << (std::ostream& os, const FullHelmholtz<d,dT,s0,s1,J0>& M)
  {
    M.print(os);
    return os;
  }

}
