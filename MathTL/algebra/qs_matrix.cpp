// implementation for qs_matrix.h

namespace MathTL
{
  template <class C>
  QuasiStationaryMatrix<C>::QuasiStationaryMatrix()
    : j0_(0), mj0_(2), nj0_(1),
      ML_(), MR_(),
      bandL_(), bandR_(),
      offsetL_(0), offsetR_(0),
      factor_(0.0)
  {
    j_ = j0_;
  };

  template <class C>
  QuasiStationaryMatrix<C>
  ::QuasiStationaryMatrix(const int j0,
			  const size_type mj0, const size_type nj0,
			  const Matrix<C>& ML, const Matrix<C>& MR,
			  const Vector<C>& bandL, const Vector<C>& bandR,
			  const size_type offsetL, const size_type offsetR,
			  const double factor)
    : j0_(j0), mj0_(mj0), nj0_(nj0),
      ML_(ML), MR_(MR),
      bandL_(bandL), bandR_(bandR),
      offsetL_(offsetL), offsetR_(offsetR),
      factor_(factor)
  {
    j_ = j0_;
  }

  template <class C>
  QuasiStationaryMatrix<C>&
  QuasiStationaryMatrix<C>::operator = (const QuasiStationaryMatrix<C>& M)
  {
    j0_ = M.j0_;
    mj0_ = M.mj0_;
    nj0_ = M.nj0_;
    ML_ = M.ML_;
    MR_ = M.MR_;
    bandL_ = M.bandL_;
    bandR_ = M.bandR_;
    offsetL_ = M.offsetL_;
    offsetR_ = M.offsetR_;
    factor_ = M.factor_;
    j_ = M.j_;
    return *this;
  }

  template <class C>
  inline
  const typename QuasiStationaryMatrix<C>::size_type
  QuasiStationaryMatrix<C>::row_dimension() const
  {
    return mj0_-(1<<(j0_+1))+(1<<(j_+1));
  }  

  template <class C>
  inline
  const typename QuasiStationaryMatrix<C>::size_type
  QuasiStationaryMatrix<C>::column_dimension() const
  {
    return nj0_-(1<<j0_)+(1<<j_);
  }  

  template <class C>
  const C QuasiStationaryMatrix<C>::get_entry(const size_type row, const size_type column) const
  {
    assert(row < row_dimension() && column < column_dimension());
    
    // for readability:
    const size_type ml = ML_.row_dimension();
    const size_type nl = ML_.column_dimension();
    const size_type mr = MR_.row_dimension();
    const size_type nr = MR_.column_dimension();
    const size_type m = row_dimension();
    const size_type n = column_dimension();

    if (column < nl) { // left block column region
      if (row < ml)
	return factor_ * ML_.get_entry(row, column);
    } else {
      if (column < n/2) { // left band column region
	const size_type colbegin = offsetL_+2*(column-nl);
	if (row >= colbegin && row < colbegin+bandL_.size())
	  return factor_ * bandL_[row-colbegin];
      } else {
	if (column >= n-nr) { // right block column region
	  if (row >= m-mr)
	    return factor_ * MR_.get_entry(row-(m-mr), column-(n-nr));
	} else { // right band column region
	  const size_type colendplus1 = m-offsetR_-2*(n-nr-1-column);
	  if (row < colendplus1 && row >= colendplus1-bandR_.size())
	    return factor_ * bandR_[(row+bandR_.size())-colendplus1];
	}
      }
    }

    return 0;
  }

  template <class C>
  void QuasiStationaryMatrix<C>::set_level(const int j) const
  {
    assert(j >= j0_);
    j_ = j;
  }

  template <class C>
  template <class VECTOR>
  void QuasiStationaryMatrix<C>::apply(const VECTOR& x, VECTOR& Mx,
				       const size_type x_offset,
				       const size_type Mx_offset,
				       const bool add_to) const
  {
    assert(x.size() >= x_offset + column_dimension());
    assert(Mx.size() >= Mx_offset + row_dimension());
    
    // for readability:
    const size_type ml = ML_.row_dimension();
    const size_type nl = ML_.column_dimension();
    const size_type mr = MR_.row_dimension();
    const size_type nr = MR_.column_dimension();
    const size_type m = row_dimension();
    const size_type n = column_dimension();

    if (add_to) {
      for (size_type i(0); i < m; i++) {
	C help(0);
	
	// contribution from upper left corner block
	if (i < ml)
	  for (size_type j(0); j < nl; j++)
	    help += ML_.get_entry(i, j) * x[x_offset + j];
	
	// contribution from left band
	const size_type jbeginlefthelp = (i+2*nl+1)-std::min(bandL_.size()+offsetL_,i+2*nl+1);
	for (size_type j = std::max(nl, jbeginlefthelp-jbeginlefthelp/2);
	     j <= std::min(n/2-1, ((i+2*nl)-offsetL_)/2); j++)
	  help += bandL_[i-offsetL_-2*(j-nl)] * x[x_offset + j];
	
	// contribution from right band
	const size_type jbeginrighthelp = (i+offsetR_+2*(n-nr-1)+1)-m;
	for (size_type j = std::max(n/2, jbeginrighthelp-jbeginrighthelp/2);
	     j <= std::min(n-nr-1, ((i+offsetR_+2*(n-nr-1)+bandR_.size())-m)/2); j++)
	  help += bandR_[i-(m-offsetR_-2*(n-nr-1-j)-bandR_.size())] * x[x_offset + j];
	
	// contribution from lower right corner block
	if (i >= m-mr)
	  for (size_type j(0); j < nr; j++)
	    help += MR_.get_entry(i-(m-mr), j) * x[x_offset + n-nr+j];
	
	Mx[Mx_offset + i] += factor_ * help;
      }
    } else {
      for (size_type i(0); i < m; i++) {
	C help(0);
	
	// contribution from upper left corner block
	if (i < ml)
	  for (size_type j(0); j < nl; j++)
	    help += ML_.get_entry(i, j) * x[x_offset + j];
	
	// contribution from left band
	const size_type jbeginlefthelp = (i+2*nl+1)-std::min(bandL_.size()+offsetL_,i+2*nl+1);
	for (size_type j = std::max(nl, jbeginlefthelp-jbeginlefthelp/2);
	     j <= std::min(n/2-1, ((i+2*nl)-offsetL_)/2); j++)
	  help += bandL_[i-offsetL_-2*(j-nl)] * x[x_offset + j];
	
	// contribution from right band
	const size_type jbeginrighthelp = (i+offsetR_+2*(n-nr-1)+1)-m;
	for (size_type j = std::max(n/2, jbeginrighthelp-jbeginrighthelp/2);
	     j <= std::min(n-nr-1, ((i+offsetR_+2*(n-nr-1)+bandR_.size())-m)/2); j++)
	  help += bandR_[i-(m-offsetR_-2*(n-nr-1-j)-bandR_.size())] * x[x_offset + j];
	
	// contribution from lower right corner block
	if (i >= m-mr)
	  for (size_type j(0); j < nr; j++)
	    help += MR_.get_entry(i-(m-mr), j) * x[x_offset + n-nr+j];
	
	Mx[Mx_offset + i] = factor_ * help;
      }
    }
  }

  template <class C>
  void
  QuasiStationaryMatrix<C>::apply(const std::map<size_type, C>& x, std::map<size_type, C>& Mx,
				  const size_type x_offset,
				  const size_type Mx_offset,
				  const bool add_to) const
  {
    // for readability:
    const size_type ml = ML_.row_dimension();
    const size_type nl = ML_.column_dimension();
    const size_type mr = MR_.row_dimension();
    const size_type nr = MR_.column_dimension();
    const size_type m = row_dimension();
    const size_type n = column_dimension();

    if (!add_to) {
      // clear the range we want to write to
      for (typename std::map<size_type,C>::iterator it(Mx.begin()); it != Mx.end();) {
	if (it->first >= Mx_offset && it->first < Mx_offset+m)
	  Mx.erase(it++);
	else
	  ++it;
      }
    }
    
    for (typename std::map<size_type,C>::const_iterator it(x.begin()); it != x.end(); ++it) {
      // contribution from upper left corner block
      if (it->first >= x_offset && it->first < x_offset+nl) {
	for (size_type i(0); i < ml; i++)
	  Mx[Mx_offset + i] +=
	    factor_ * ML_.get_entry(i, it->first-x_offset) * it->second;
      }
      
      // contribution from left band
      if (it->first >= x_offset+nl && it->first < x_offset+n/2) {
	const size_type ibegin = offsetL_+2*(it->first-x_offset-nl);
	for (size_type i(ibegin); i < ibegin+bandL_.size(); i++) {
	  Mx[Mx_offset + i] +=
	    factor_ * bandL_[i-ibegin] * it->second;
	}
      }
      
      // contribution from right band
      if (it->first >= x_offset+n/2 && it->first < x_offset+n-nr) {
	const size_type iendplus1 = m-offsetR_-2*(n+x_offset-nr-1-it->first);
	for (size_type i(iendplus1-bandR_.size()); i < iendplus1; i++)
	  Mx[Mx_offset + i] +=
	    factor_ * bandR_[(i+bandR_.size())-iendplus1] * it->second;
      }
      
      // contribution from lower right corner block
      if (it->first >= x_offset+n-nr && it->first < x_offset+n) {
	for (size_type i(m-mr); i < m; i++)
	  Mx[Mx_offset + i] +=
	    factor_ * MR_.get_entry(i-(m-mr), it->first-x_offset-(n-nr)) * it->second;
      }
    } 

    // remove unnecessary zeros
    for (typename std::map<size_type,C>::iterator it(Mx.begin()); it != Mx.end();) {
      if (it->second == C(0))
	Mx.erase(it++);
      else
	++it;
    }  
  } 
  
  template <class C>
  void
  QuasiStationaryMatrix<C>::apply_central_block
  (const std::map<size_type, C>& x, std::map<size_type, C>& Mx,
   const size_type x_offset,
   const size_type Mx_offset,
   const bool add_to) const
  {
    // for readability:
    const size_type ml = ML_.row_dimension();
    const size_type nl = ML_.column_dimension();
    const size_type mr = MR_.row_dimension();
    const size_type nr = MR_.column_dimension();
    const size_type m = row_dimension();
    const size_type n = column_dimension();

    if (!add_to) {
      // clear the range we want to write to
      for (typename std::map<size_type,C>::iterator it(Mx.begin()); it != Mx.end();) {
	if (it->first >= Mx_offset && it->first < Mx_offset+(m-2))
	  Mx.erase(it++);
	else
	  ++it;
      }
    }
    
    for (typename std::map<size_type,C>::const_iterator it(x.begin()); it != x.end(); ++it) {
      // contribution from upper left corner block
      if (it->first >= x_offset && it->first < x_offset+(nl-1)) {
	for (size_type i(0); i < ml-1; i++)
	  Mx[Mx_offset + i] +=
	    factor_ * ML_.get_entry(i+1, it->first-x_offset+1) * it->second;
      }
      
      // contribution from left band
      if (it->first >= x_offset+(nl-1) && it->first < x_offset+(n-2)/2) {
	const size_type ibegin = offsetL_-1+2*(it->first-x_offset-(nl-1));
	for (size_type i(ibegin); i < ibegin+bandL_.size(); i++) {
	  Mx[Mx_offset + i] +=
	    factor_ * bandL_[i-ibegin] * it->second;
	}
      }
      
      // contribution from right band
      if (it->first >= x_offset+(n-2)/2 && it->first < x_offset+(n-2)-(nr-1)) {
	const size_type iendplus1 = (m-2)-(offsetR_-1)-2*((n-2)+x_offset-(nr-1)-1-it->first);
	for (size_type i(iendplus1-bandR_.size()); i < iendplus1; i++)
	  Mx[Mx_offset + i] +=
	    factor_ * bandR_[(i+bandR_.size())-iendplus1] * it->second;
      }
      
      // contribution from lower right corner block
      if (it->first >= x_offset+(n-2)-(nr-1) && it->first < x_offset+(n-2)) {
	for (size_type i((m-2)-(mr-1)); i < m-2; i++)
	  Mx[Mx_offset + i] +=
	    factor_ * MR_.get_entry(i-((m-2)-(mr-1)), it->first-x_offset-((n-2)-(nr-1))) * it->second;
      }
    } 
    
    // remove unnecessary zeros
    for (typename std::map<size_type,C>::iterator it(Mx.begin()); it != Mx.end();) {
      if (it->second == C(0))
	Mx.erase(it++);
      else
	++it;
    }    
  }

  template <class C>
  void
  QuasiStationaryMatrix<C>::apply_central_columns
  (const std::map<size_type, C>& x, std::map<size_type, C>& Mx,
   const size_type x_offset,
   const size_type Mx_offset,
   const bool add_to) const
  {
    // for readability:
    const size_type ml = ML_.row_dimension();
    const size_type nl = ML_.column_dimension();
    const size_type mr = MR_.row_dimension();
    const size_type nr = MR_.column_dimension();
    const size_type m = row_dimension();
    const size_type n = column_dimension();

    if (!add_to) {
      // clear the range we want to write to
      for (typename std::map<size_type,C>::iterator it(Mx.begin()); it != Mx.end();) {
	if (it->first >= Mx_offset && it->first < Mx_offset+m)
	  Mx.erase(it++);
	else
	  ++it;
      }
    }
    
    for (typename std::map<size_type,C>::const_iterator it(x.begin()); it != x.end(); ++it) {
      // contribution from upper left corner block
      if (it->first >= x_offset && it->first < x_offset+(nl-1)) {
	for (size_type i(0); i < ml; i++)
	  Mx[Mx_offset + i] +=
	    factor_ * ML_.get_entry(i, it->first-x_offset+1) * it->second;
      }
      
      // contribution from left band
      if (it->first >= x_offset+(nl-1) && it->first < x_offset+(n-2)/2) {
	const size_type ibegin = offsetL_+2*(it->first-x_offset-(nl-1));
	for (size_type i(ibegin); i < ibegin+bandL_.size(); i++) {
	  Mx[Mx_offset + i] +=
	    factor_ * bandL_[i-ibegin] * it->second;
	}
      }
      
      // contribution from right band
      if (it->first >= x_offset+(n-2)/2 && it->first < x_offset+(n-2)-(nr-1)) {
	const size_type iendplus1 = m-offsetR_-2*((n-2)+x_offset-(nr-1)-1-it->first);
	for (size_type i(iendplus1-bandR_.size()); i < iendplus1; i++)
	  Mx[Mx_offset + i] +=
	    factor_ * bandR_[(i+bandR_.size())-iendplus1] * it->second;
      }
      
      // contribution from lower right corner block
      if (it->first >= x_offset+(n-2)-(nr-1) && it->first < x_offset+(n-2)) {
	for (size_type i(m-mr); i < m; i++)
	  Mx[Mx_offset + i] +=
	    factor_ * MR_.get_entry(i-(m-mr), it->first-x_offset-((n-2)-(nr-1))) * it->second;
      }
    } 
    
    // remove unnecessary zeros
    for (typename std::map<size_type,C>::iterator it(Mx.begin()); it != Mx.end();) {
      if (it->second == C(0))
	Mx.erase(it++);
      else
	++it;
    }    
  }

  template <class C>
  template <class VECTOR>
  void QuasiStationaryMatrix<C>::apply_transposed(const VECTOR& x, VECTOR& Mtx,
						  const size_type x_offset,
						  const size_type Mtx_offset,
						  const bool add_to) const
  {
    assert(x.size() >= x_offset + row_dimension());
    assert(Mtx.size() >= Mtx_offset + column_dimension());
    
    // for readability:
    const size_type ml = ML_.row_dimension();
    const size_type nl = ML_.column_dimension();
    const size_type mr = MR_.row_dimension();
    const size_type nr = MR_.column_dimension();
    const size_type m = row_dimension();
    const size_type n = column_dimension();

    if (add_to) {
      // contribution from upper left corner block
      for (size_type i(0); i < nl; i++) {
	C help(0);
	for (size_type j(0); j < ml; j++) {
	  help += ML_.get_entry(j, i) * x[x_offset + j];
	}
	Mtx[Mtx_offset + i] += factor_ * help;
      }
      
      // contribution from left band
      for (size_type i(nl); i < n/2; i++) {
	C help(0);
	const size_type jbegin = offsetL_+2*(i-nl);
	for (size_type j(jbegin); j < jbegin+bandL_.size(); j++)
	  help += bandL_[j-jbegin] * x[x_offset + j];
	Mtx[Mtx_offset + i] += factor_ * help;
      }
      
      // contribution from right band
      for (size_type i(n/2); i < n-nr; i++) {
	C help(0);
	const size_type jendplus1 = m-offsetR_-2*(n-nr-1-i);
	for (size_type j(jendplus1-bandR_.size()); j < jendplus1; j++)
	  help += bandR_[(j+bandR_.size())-jendplus1] * x[x_offset + j];
	Mtx[Mtx_offset + i] += factor_ * help;
      }
      
      // contribution from lower right corner block
      for (size_type i(n-nr); i < n; i++) {
	C help(0);
	for (size_type j(m-mr); j < m; j++) {
	  help += MR_.get_entry(j-(m-mr), i-(n-nr)) * x[x_offset + j];
	}
	Mtx[Mtx_offset + i] += factor_ * help;
      }
    } else {
      // contribution from upper left corner block
      for (size_type i(0); i < nl; i++) {
	C help(0);
	for (size_type j(0); j < ml; j++) {
	  help += ML_.get_entry(j, i) * x[x_offset + j];
	}
	Mtx[Mtx_offset + i] = factor_ * help;
      }
      
      // contribution from left band
      for (size_type i(nl); i < n/2; i++) {
	C help(0);
	const size_type jbegin = offsetL_+2*(i-nl);
	for (size_type j(jbegin); j < jbegin+bandL_.size(); j++)
	  help += bandL_[j-jbegin] * x[x_offset + j];
	Mtx[Mtx_offset + i] = factor_ * help;
      }
      
      // contribution from right band
      for (size_type i(n/2); i < n-nr; i++) {
	C help(0);
	const size_type jendplus1 = m-offsetR_-2*(n-nr-1-i);
	for (size_type j(jendplus1-bandR_.size()); j < jendplus1; j++)
	  help += bandR_[(j+bandR_.size())-jendplus1] * x[x_offset + j];
	Mtx[Mtx_offset + i] = factor_ * help;
      }
      
      // contribution from lower right corner block
      for (size_type i(n-nr); i < n; i++) {
	C help(0);
	for (size_type j(m-mr); j < m; j++) {
	  help += MR_.get_entry(j-(m-mr), i-(n-nr)) * x[x_offset + j];
	}
	Mtx[Mtx_offset + i] = factor_ * help;
      }
    }
  }
  
  template <class C>
  void
  QuasiStationaryMatrix<C>::apply_transposed(const std::map<size_type, C>& x,
					     std::map<size_type, C>& Mtx,
					     const size_type x_offset,
					     const size_type Mtx_offset,
					     const bool add_to) const
  {
    // for readability:
    const size_type ml = ML_.row_dimension();
    const size_type nl = ML_.column_dimension();
    const size_type mr = MR_.row_dimension();
    const size_type nr = MR_.column_dimension();
    const size_type m = row_dimension();
    const size_type n = column_dimension();
    
    if (!add_to) {
      // clear the range we want to write to
      for (typename std::map<size_type,C>::iterator it(Mtx.begin()); it != Mtx.end();) {
	if (it->first >= Mtx_offset && it->first < Mtx_offset+n)
	  Mtx.erase(it++);
	else
	  ++it;
      }
    }

    for (typename std::map<size_type,C>::const_iterator it(x.begin()); it != x.end(); ++it) {
      // contribution from upper left corner block
      if (it->first >= x_offset && it->first < x_offset+ml) {
	for (size_type i(0); i < nl; i++) {
	  Mtx[Mtx_offset+i] +=
	    factor_ * ML_.get_entry(it->first-x_offset, i) * it->second;
	}
      }

      // contribution from left band
      const size_type ibeginlefthelp
	= (it->first-x_offset+2*nl+1)-std::min(bandL_.size()+offsetL_,it->first-x_offset+2*nl+1);
      for (size_type i = std::max(nl, ibeginlefthelp-ibeginlefthelp/2);
	   i <= std::min(n/2-1, ((it->first-x_offset+2*nl)-offsetL_)/2); i++)
	Mtx[Mtx_offset+i] +=
	  factor_ * bandL_[it->first-x_offset-offsetL_-2*(i-nl)] * it->second;
      
      // contribution from right band
      const size_type ibeginrighthelp
	= (it->first-x_offset+offsetR_+2*(n-nr-1)+1)-m;
      for (size_type i = std::max(n/2, ibeginrighthelp-ibeginrighthelp/2);
	   i <= std::min(n-nr-1, ((it->first-x_offset+offsetR_+2*(n-nr-1)+bandR_.size())-m)/2); i++)
	Mtx[Mtx_offset+i] +=
	  factor_ * bandR_[it->first-x_offset-(m-offsetR_-2*(n-nr-1-i)-bandR_.size())] * it->second;
      
      // contribution from lower right corner block
      if (it->first >= x_offset+(m-mr) && it->first < x_offset+m) {
	for (size_type i(0); i < nr; i++) {
	  Mtx[Mtx_offset+i+(n-nr)] +=
	    factor_ * MR_.get_entry(it->first-x_offset-(m-mr), i) * it->second;
	}
      }
    }

    // remove unnecessary zeros
    for (typename std::map<size_type,C>::iterator it(Mtx.begin()); it != Mtx.end();) {
      if (it->second == C(0))
	Mtx.erase(it++);
      else
	++it;
    }      
  }

  template <class C>
  void
  QuasiStationaryMatrix<C>::apply_central_block_transposed
  (const std::map<size_type, C>& x, std::map<size_type, C>& Mtx,
   const size_type x_offset,
   const size_type Mtx_offset,
   const bool add_to) const
  {
    // for readability:
    const size_type ml = ML_.row_dimension();
    const size_type nl = ML_.column_dimension();
    const size_type mr = MR_.row_dimension();
    const size_type nr = MR_.column_dimension();
    const size_type m = row_dimension();
    const size_type n = column_dimension();
    
    if (!add_to) {
      // clear the range we want to write to
      for (typename std::map<size_type,C>::iterator it(Mtx.begin()); it != Mtx.end();) {
	if (it->first >= Mtx_offset && it->first < Mtx_offset+(n-2))
	  Mtx.erase(it++);
	else
	  ++it;
      }
    }

    for (typename std::map<size_type,C>::const_iterator it(x.begin()); it != x.end(); ++it) {
      // contribution from upper left corner block
      if (it->first >= x_offset && it->first < x_offset+(ml-1)) {
	for (size_type i(0); i < nl-1; i++) {
	  Mtx[Mtx_offset+i] +=
	    factor_ * ML_.get_entry(it->first-x_offset+1, i+1) * it->second;
	}
      }

      // contribution from left band
      const size_type ibeginlefthelp
 	= (it->first-x_offset+2*(nl-1)+1)-std::min(bandL_.size()+(offsetL_-1),it->first-x_offset+2*(nl-1)+1);
      for (size_type i = std::max(nl-1, ibeginlefthelp-ibeginlefthelp/2);
 	   i <= std::min((n-2)/2-1, ((it->first-x_offset+2*(nl-1))-(offsetL_-1))/2); i++)
 	Mtx[Mtx_offset+i] +=
 	  factor_ * bandL_[it->first-x_offset-(offsetL_-1)-2*(i-(nl-1))] * it->second;
      
      // contribution from right band
      const size_type ibeginrighthelp
 	= (it->first-x_offset+(offsetR_-1)+2*((n-2)-(nr-1)-1)+1)-(m-2);
      for (size_type i = std::max((n-2)/2, ibeginrighthelp-ibeginrighthelp/2);
 	   i <= std::min((n-2)-(nr-1)-1, ((it->first-x_offset+(offsetR_-1)+2*((n-2)-(nr-1)-1)+bandR_.size())-(m-2))/2); i++)
 	Mtx[Mtx_offset+i] +=
 	  factor_ * bandR_[it->first-x_offset-((m-2)-(offsetR_-1)-2*((n-2)-(nr-1)-1-i)-bandR_.size())] * it->second;
      
      // contribution from lower right corner block
      if (it->first >= x_offset+((m-2)-(mr-1)) && it->first < x_offset+(m-2)) {
 	for (size_type i(0); i < nr-1; i++) {
 	  Mtx[Mtx_offset+i+((n-2)-(nr-1))] +=
 	    factor_ * MR_.get_entry(it->first-x_offset-((m-2)-(mr-1)), i) * it->second;
 	}
      }
    }
    
    // remove unnecessary zeros
    for (typename std::map<size_type,C>::iterator it(Mtx.begin()); it != Mtx.end();) {
      if (it->second == C(0))
	Mtx.erase(it++);
      else
	++it;
    }      
  }
  
  template <class C>
  void
  QuasiStationaryMatrix<C>::apply_central_columns_transposed
  (const std::map<size_type, C>& x, std::map<size_type, C>& Mtx,
   const size_type x_offset,
   const size_type Mtx_offset,
   const bool add_to) const
  {
    // for readability:
    const size_type ml = ML_.row_dimension();
    const size_type nl = ML_.column_dimension();
    const size_type mr = MR_.row_dimension();
    const size_type nr = MR_.column_dimension();
    const size_type m = row_dimension();
    const size_type n = column_dimension();
    
    if (!add_to) {
      // clear the range we want to write to
      for (typename std::map<size_type,C>::iterator it(Mtx.begin()); it != Mtx.end();) {
	if (it->first >= Mtx_offset && it->first < Mtx_offset+(n-2))
	  Mtx.erase(it++);
	else
	  ++it;
      }
    }

    for (typename std::map<size_type,C>::const_iterator it(x.begin()); it != x.end(); ++it) {
      // contribution from upper left corner block
      if (it->first >= x_offset && it->first < x_offset+ml) {
	for (size_type i(0); i < nl-1; i++) {
	  Mtx[Mtx_offset+i] +=
	    factor_ * ML_.get_entry(it->first-x_offset, i+1) * it->second;
	}
      }

      // contribution from left band
      const size_type ibeginlefthelp
 	= (it->first-x_offset+2*(nl-1)+1)-std::min(bandL_.size()+offsetL_,it->first-x_offset+2*(nl-1)+1);
      for (size_type i = std::max(nl-1, ibeginlefthelp-ibeginlefthelp/2);
 	   i <= std::min((n-2)/2-1, ((it->first-x_offset+2*(nl-1))-offsetL_)/2); i++)
 	Mtx[Mtx_offset+i] +=
 	  factor_ * bandL_[it->first-x_offset-offsetL_-2*(i-(nl-1))] * it->second;
      
      // contribution from right band
      const size_type ibeginrighthelp
 	= (it->first-x_offset+offsetR_+2*((n-2)-(nr-1)-1)+1)-m;
      for (size_type i = std::max((n-2)/2, ibeginrighthelp-ibeginrighthelp/2);
 	   i <= std::min((n-2)-(nr-1)-1, ((it->first-x_offset+offsetR_+2*((n-2)-(nr-1)-1)+bandR_.size())-m)/2); i++)
 	Mtx[Mtx_offset+i] +=
 	  factor_ * bandR_[it->first-x_offset-(m-offsetR_-2*((n-2)-(nr-1)-1-i)-bandR_.size())] * it->second;
      
      // contribution from lower right corner block
      if (it->first >= x_offset+(m-mr) && it->first < x_offset+m) {
 	for (size_type i(0); i < nr-1; i++) {
 	  Mtx[Mtx_offset+i+((n-2)-(nr-1))] +=
 	    factor_ * MR_.get_entry(it->first-x_offset-(m-mr), i) * it->second;
 	}
      }
    }
    
    // remove unnecessary zeros
    for (typename std::map<size_type,C>::iterator it(Mtx.begin()); it != Mtx.end();) {
      if (it->second == C(0))
	Mtx.erase(it++);
      else
	++it;
    }      
  }
  
  template <class C>
  void
  QuasiStationaryMatrix<C>::to_sparse(SparseMatrix<C>& S) const
  {
    // for readability:
    const size_type nl = ML_.column_dimension();
    const size_type mr = MR_.row_dimension();
    const size_type nr = MR_.column_dimension();
    const size_type m = row_dimension();
    const size_type n = column_dimension();

    S.resize(m, n);

    // corner blocks
    if (nl>0) S.set_block(0, 0, ML_);
    if (nr>0) S.set_block(m-mr, n-nr, MR_);
    
    for (unsigned int i(0); i < m; i++) {
      // left band
      const size_type jbeginlefthelp = (i+2*nl+1)-std::min(bandL_.size()+offsetL_,i+2*nl+1);
      for (size_type j = std::max(nl, jbeginlefthelp-jbeginlefthelp/2);
	   j <= std::min(n/2-1, ((i+2*nl)-offsetL_)/2); j++) {
	S.set_entry(i, j, bandL_[i-offsetL_-2*(j-nl)]);
      }
      
      // right band
      const size_type jbeginrighthelp = (i+offsetR_+2*(n-nr-1)+1)-m;
      for (size_type j = std::max(n/2, jbeginrighthelp-jbeginrighthelp/2);
	   j <= std::min(n-nr-1, ((i+offsetR_+2*(n-nr-1)+bandR_.size())-m)/2); j++)
	S.set_entry(i, j, bandR_[i-(m-offsetR_-2*(n-nr-1-j)-bandR_.size())]);
    }

    S.scale(factor_);
  }
    
  template <class C>
  void QuasiStationaryMatrix<C>::print(std::ostream &os,
				       const unsigned int tabwidth,
				       const unsigned int precision) const
  {
    if (row_dimension() == 0)
      os << "[]" << std::endl; // Matlab style
    else
      {
	unsigned int old_precision = os.precision(precision);
	for (typename QuasiStationaryMatrix<C>::size_type i(0); i < row_dimension(); ++i)
	  {
	    for (typename QuasiStationaryMatrix<C>::size_type j(0); j < column_dimension(); ++j)
	      os << std::setw(tabwidth) << std::setprecision(precision)
		 << get_entry(i, j);
	    os << std::endl;
	  }
	os.precision(old_precision);
      }
  }

  template <class C>
  inline
  std::ostream& operator << (std::ostream& os, const QuasiStationaryMatrix<C>& M)
  {
    M.print(os);
    return os;
  }

  //
  //
  // implementation for PeriodicQuasiStationaryMatrix

  template <class C>
  PeriodicQuasiStationaryMatrix<C>
  ::PeriodicQuasiStationaryMatrix(const int j0,
				  const int offset,
				  const Array1D<C>& band,
				  const double factor)
    : j0_(j0), offset_(offset), band_(band), factor_(factor)
  {
    j_ = j0_;
  }
  
  template <class C>
  void PeriodicQuasiStationaryMatrix<C>::set_level(const int j) const
  {
    assert(j >= j0_);
    j_ = j;
  }

  template <class C>
  void PeriodicQuasiStationaryMatrix<C>::print(std::ostream &os,
					       const unsigned int tabwidth,
					       const unsigned int precision) const
  {
    if (row_dimension() == 0)
      os << "[]" << std::endl; // Matlab style
    else
      {
	unsigned int old_precision = os.precision(precision);
	for (typename QuasiStationaryMatrix<C>::size_type i(0); i < row_dimension(); ++i)
	  {
	    for (typename QuasiStationaryMatrix<C>::size_type j(0); j < column_dimension(); ++j)
	      os << std::setw(tabwidth) << std::setprecision(precision)
		 << get_entry(i, j);
	    os << std::endl;
	  }
	os.precision(old_precision);
      }
  }

  template <class C>
  inline
  std::ostream& operator << (std::ostream& os, const PeriodicQuasiStationaryMatrix<C>& M)
  {
    M.print(os);
    return os;
  }
  
  template <class C>
  const C PeriodicQuasiStationaryMatrix<C>::get_entry(const size_type row, const size_type column) const
  {
    assert(row < row_dimension() && column < column_dimension());
 
    int mask_index = row-2*(int)column; // row relative to a0
    if (mask_index < offset_) {
      mask_index += (1<<(j_+1));
    } else {
      if (mask_index >= offset_+(int)band_.size()) {
	mask_index -= (1<<(j_+1));
      }
    }

    if (mask_index >= offset_ && mask_index < offset_+(int)band_.size())
      return factor_ * band_[mask_index-offset_];
    
    return 0;
  }
    
  template <class C>
  PeriodicQuasiStationaryMatrix<C>&
  PeriodicQuasiStationaryMatrix<C>::operator = (const PeriodicQuasiStationaryMatrix<C>& M)
  {
    j0_ = M.j0_;
    band_ = M.band_;
    offset_ = M.offset_;
    factor_ = M.factor_;
    j_ = M.j_;
    return *this;
  }

  template <class C>
  template <class VECTOR>
  void PeriodicQuasiStationaryMatrix<C>::apply(const VECTOR& x, VECTOR& Mx,
					       const size_type x_offset,
					       const size_type Mx_offset,
					       const bool add_to) const
  {
    assert(x.size() >= x_offset + column_dimension());
    assert(Mx.size() >= Mx_offset + row_dimension());

    // for readability:
    const size_type m = row_dimension();
    const size_type n = column_dimension();

    if (!add_to) {
      // clear the range we want to write to
      for (size_type i = Mx_offset; i < Mx_offset+m; i++)
	Mx[i] = 0;
    }
    
    // traverse all columns
    for (size_type j(0); j < n; j++) {
      // per column, _all_ entries of the band will occur, likely at "periodized" rows
      for (size_type i(0); i < band_.size(); i++) {
 	Mx[Mx_offset+dyadic_modulo(offset_+(int)i+2*j, j_+1)]
	  += factor_ * band_[i] * x[x_offset+j];
      }
    } 
  }

  template <class C>
  void
  PeriodicQuasiStationaryMatrix<C>::to_sparse(SparseMatrix<C>& S) const
  {
    // for readability:
    const size_type m = row_dimension();
    const size_type n = column_dimension();

    S.resize(m, n);

    // insert the entries columnwise
    for (size_type j(0); j < n; j++) {
      // per column, _all_ entries of the band will occur, likely at "periodized" rows
      for (size_type i(0); i < band_.size(); i++) {
	S.set_entry(dyadic_modulo(offset_+(int)i+2*j, j_+1), j, band_[i]);
      }
    } 
    
    S.scale(factor_);
  }
  
  template <class C>
  template <class VECTOR>
  void PeriodicQuasiStationaryMatrix<C>::apply_transposed(const VECTOR& x, VECTOR& Mtx,
							  const size_type x_offset,
							  const size_type Mtx_offset,
							  const bool add_to) const
  {
    assert(x.size() >= x_offset + row_dimension());
    assert(Mtx.size() >= Mtx_offset + column_dimension());
    
    // for readability:
    const size_type n = column_dimension();

    if (!add_to) {
      // clear the range we want to write to
      for (size_type i = Mtx_offset; i < Mtx_offset+n; i++)
	Mtx[i] = 0;
    }
    
    // traverse all rows
    for (size_type i(0); i < n; i++) {
      // per row, _all_ entries of the band will occur, likely at "periodized" columns
      for (size_type j(0); j < band_.size(); j++) {
 	Mtx[Mtx_offset+i]
	  += factor_ * band_[j] * x[x_offset+dyadic_modulo(offset_+(int)j+2*i, j_+1)];
      }
    } 
  }
  
}
