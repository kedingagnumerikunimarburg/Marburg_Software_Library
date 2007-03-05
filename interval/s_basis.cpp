// implementation for s_basis

#include <cassert>
#include <cmath>

#include <Rd/dhjk_mask.h>

using namespace std;

namespace WaveletTL
{
  SBasis::SBasis()
  {
    setup();
  }

  void
  SBasis::setup()
  {
    // setup dual generator refinement matrix boundary blocks
    MLT = FixedMatrix<double,8,2>("1.0 -7.5 0.0 0.5 1.109375 0.9609375 0.078125 1.1875 0.5 3.09375 -0.1875 -1.15625 -0.109375 -0.6796875 0.078125 0.484375");
    MRT = FixedMatrix<double,8,2>("-0.109375 0.6796875 -0.078125 0.484375 0.5 -3.09375 0.1875 -1.15625 1.109375 -0.9609375 -0.078125 1.1875 1.0 7.5 0.0 0.5");

    Mj1L = FixedMatrix<double,6,2>("0.28125 0.03125 -0.84375 1.03125 -0.5 0.0 1.875 -0.125 0.21875 -0.03125 -0.09375 0.03125");
    Mj1R = FixedMatrix<double,6,2>("0.21875 0.03125 0.09375 0.03125 -0.5 0.0 -1.875 -0.125 0.28125 -0.03125 0.84375 1.03125");
    Mj1I = FixedMatrix<double,10,2>("0.068359375 -0.025390625" \
                                    "0.005859375 -0.001953125" \
                                    "-0.25 0.09375" \
                                    "-0.7734375 0.2890625" \
                                    "0.36328125 0.0" \
                                    "0.0 0.71484375" \
                                    "-0.25 -0.09375" \
                                    "0.7734375 0.2890625" \
                                    "0.068359375 0.025390625" \
                                    "-0.005859375 -0.001953125");

    MTj1I = FixedMatrix<double, 6, 2>("-0.5 0.75 -0.25 0.25 1.0 0.0 0.0 1.0 -0.5 -0.75 0.25 0.25");

    GW = FixedMatrix<double,4,2>("-0.5 0.25 -0.75 0.25 -0.5 -0.25 0.75 0.25");
  }


  inline
  SBasis::Index
  SBasis::first_generator(const int j) const
  {
    assert(j >= j0());
    return Index(j, E_GENERATOR, DeltaLmin(), 0, this);
  }
  
  inline
  SBasis::Index
  SBasis::last_generator(const int j) const
  {
    assert(j >= j0());
    return Index(j, E_GENERATOR, DeltaRmax(j), number_of_components-1, this);
  }

  inline
  SBasis::Index
  SBasis::first_wavelet(const int j) const
  {
    assert(j >= j0());
    return Index(j, E_WAVELET, Nablamin(), 0, this);
  }
  
  inline
  SBasis::Index
  SBasis::last_wavelet(const int j) const
  {
    assert(j >= j0());
    return Index(j, E_WAVELET, Nablamax(j), number_of_components-1, this);
  }


  /* DECOMPOSE methods */
  void
  SBasis::decompose_1(const SBasis::Index& lambda, InfiniteVector<double, SBasis::Index>& c) const
  {
    assert(lambda.e() == E_GENERATOR);
    assert(lambda.j() > j0());

    c.clear();

    SBasis::Index mu;
    SBasis::Index::translation_type k_now = DeltaLmin(); // initialize just to satisfy the compiler

    // \Phi_j = \tilde M_{j-1, 0} \Phi_{j-1} + G_{j-1, 1}  \psi_{j-1}

    // first calculate corresponding row of \tilde M_{j,0}
    if ((lambda.k() >= DeltaLmin()) && (lambda.k() <= DeltaLmin()+MLT.row_dimension()/number_of_components-1)) { // left boundary block involved
      const int row = (lambda.k()-DeltaLmin())*number_of_components+lambda.c();
      k_now = DeltaLmin();
      for (mu = SBasis::Index(lambda.j()-1,E_GENERATOR,k_now,0,this); mu.k() == DeltaLmin(); ++mu)
        c.set_coefficient(mu, MLT.get_entry(row, mu.c()));
      if (lambda.k() > DeltaLmin()) {
        k_now++;
        for (; mu.k() == k_now; ++mu)
          c.set_coefficient(mu, mask_dual.get_coefficient(lambda.k()-DeltaLmin()-3).get_entry(mu.c(),lambda.c()));
        if (lambda.k() == DeltaLmin()+3) {
          k_now++;
          for (; mu.k() == k_now; ++mu)
            c.set_coefficient(mu, mask_dual.get_coefficient(-2).get_entry(mu.c(),lambda.c()));
        }
      }
    }
    else if ((lambda.k() >= DeltaRmax(lambda.j())-MRT.row_dimension()/number_of_components+1) && (lambda.k() <= DeltaRmax(lambda.j()))) { // right boundary block involved
      const int row = (lambda.k()-DeltaRmax(lambda.j())+MRT.row_dimension()/number_of_components-1)*number_of_components+lambda.c();
      k_now = DeltaRmax(lambda.j()-1);
      for (mu = SBasis::Index(lambda.j()-1,E_GENERATOR,k_now,number_of_components-1,this); mu.k() == k_now; --mu)
        c.set_coefficient(mu, MRT.get_entry(row, mu.c()));
      if (lambda.k() < DeltaRmax(lambda.j())) {
        k_now--;
        for (; mu.k() == k_now; --mu)
          c.set_coefficient(mu, mask_dual.get_coefficient(lambda.k()-DeltaRmax(lambda.j())+3).get_entry(mu.c(),lambda.c()));
        if (lambda.k() == DeltaRmax(lambda.j())-3) {
          k_now--;
          for (; mu.k() == k_now; --mu)
            c.set_coefficient(mu, mask_dual.get_coefficient(2).get_entry(mu.c(),lambda.c()));
        }
      }
    }
    else { // only inner blocks involved
      for (int i = 1 + ((lambda.k()-DeltaLmin())%2), k_now = DeltaLmin()+(lambda.k()-DeltaLmin()-2)/2; i >= -2; i -= 2, k_now++) {
        for (mu = SBasis::Index(lambda.j()-1,E_GENERATOR,k_now,0,this); mu.k() == k_now; ++mu)
          c.set_coefficient(mu, mask_dual.get_coefficient(i).get_entry(mu.c(),lambda.c()));
      }
    }
    c *= M_SQRT1_2;

    // now calculate corresponding row of G_{j,1} ( = \tilde M_{j,1}^T )
    if ((lambda.k()-DeltaLmin())%2 == 0) { // here only one identity block
      c.set_coefficient(SBasis::Index(lambda.j()-1,E_WAVELET,Nablamin()+(lambda.k()-DeltaLmin())/2,lambda.c(),this),1.0);
    }
    else { // Gj1W block
      const SBasis::Index::translation_type k_start = Nablamin() + (lambda.k()-DeltaLmin()-1)/2;
      const SBasis::Index mu_end = SBasis::Index(lambda.j()-1,E_WAVELET,k_start + GW.row_dimension()/number_of_components-1,number_of_components-1,this);
      for (mu = SBasis::Index(lambda.j()-1,E_WAVELET,k_start,0,this); mu <= mu_end; ++mu)
        c.set_coefficient(mu, GW.get_entry(number_of_components*(mu.k()-k_start) + mu.c(), lambda.c()));
    }
  }
  
  void
  SBasis::decompose_1(const SBasis::Index& lambda, const int jmin,
                      InfiniteVector<double, SBasis::Index>& c) const
  {
    assert(jmin >= j0());
    assert(lambda.j() >= jmin);

    InfiniteVector<double, SBasis::Index> cphi, ctmp;

    c.clear();

    if (lambda.e() == E_WAVELET)
      c.set_coefficient(lambda, 1.0); // nothing to do
    else { // generator
      cphi.set_coefficient(lambda, 1.0);
      for (int jnow = lambda.j(); jnow > jmin; jnow--) {
        // \Phi_j = \tilde M_{j-1, 0} \Phi_{j-1} + G_{j-1, 1}  \psi_{j-1}
        ctmp.clear();
        for (InfiniteVector<double, SBasis::Index>::const_iterator it(cphi.begin()), itend(cphi.end());
             it != itend; ++it) {
          InfiniteVector<double, SBasis::Index> vtmp;
          decompose_1(it.index(), vtmp);
          ctmp.add(*it, vtmp); // ctmp += (*it) * vtmp
        }
        // now we have the refined coefficient set in ctmp, all indices have |\lambda| = jnow
        // write all generator indices into cphi, add wavelet indices to c
        cphi.clear();
        for (InfiniteVector<double, SBasis::Index>::const_iterator it(ctmp.begin()), itend(ctmp.end());
             it != itend; ++it) {
          if (it.index().e() == E_WAVELET)
            c.add(*it, ctmp);
          else
            cphi.add(*it, ctmp);
        }
      }
      // copy contents of cphi to c
      for (InfiniteVector<double, SBasis::Index>::const_iterator it(cphi.begin()), itend(cphi.end());
           it != itend; ++it)
        c.set_coefficient(it.index(), *it); // there shouldn't be entries in the generators present
    }
  }

  void
  SBasis::decompose(const InfiniteVector<double, SBasis::Index>& c, const int jmin,
                    InfiniteVector<double, SBasis::Index>& v) const
  {
    v.clear();
    InfiniteVector<double, SBasis::Index> vtmp;
    for (InfiniteVector<double, SBasis::Index>::const_iterator it(c.begin()), itend(c.end());
         it != itend; ++it) {
      decompose_1(it.index(), jmin, vtmp); // calls vtmp.clear() first
      v.add(*it, vtmp);
    }
  }

  void
  SBasis::decompose_t_1(const SBasis::Index& lambda, InfiniteVector<double, SBasis::Index>& c) const
  {
    assert(lambda.e() == E_GENERATOR);
    assert(lambda.j() > j0());

    c.clear();

    // first build the corresponding row of M_{j-1,0} for the coarsed generator functions
    // we only have inner functions
    SBasis::Index mu;
    SBasis::Index::translation_type dk = ((lambda.k()-DeltaLTmin())%2 == 0) ? 1 : 0;
    SBasis::Index::translation_type k_start = DeltaLTmin() + ( (lambda.k() == DeltaLTmin()) ? -1 : (lambda.k()-DeltaLTmin()-1)/2 );
    SBasis::Index mu_end(lambda.j()-1,E_GENERATOR,min(DeltaRTmax(lambda.j()-1), k_start+dk),number_of_components-1,this);
    int i;
    for (mu = SBasis::Index(lambda.j()-1,E_GENERATOR,max(DeltaLTmin(),k_start),0,this), i = dk; mu <= mu_end; ++mu)
      c.set_coefficient(mu, mask_primal.get_coefficient(-dk+2*(mu.k()-k_start)).get_entry(mu.c(),lambda.c()));
    c *= M_SQRT1_2;

    // now build the corresponding row of M{j-1,1} for the wavelets
    if ((lambda.k() >= DeltaLTmin()) && (lambda.k() <= DeltaLTmin()+2)) { // left boundary block involved
      int row = 2*(lambda.k()-DeltaLTmin())+lambda.c();
      for (mu = SBasis::Index(lambda.j()-1,E_WAVELET,Nablamin(),0,this), i = 0; mu.k() == Nablamin(); ++mu, i++) {
        c.set_coefficient(mu, Mj1L.get_entry(row, i));
      }
      for(; mu.k() == Nablamin()+1; ++mu)
        c.set_coefficient(mu, Mj1I.get_entry(row,mu.c()));
      if (lambda.k() == DeltaLTmin()+2)
        for(; mu.k() == Nablamin()+2; ++mu)
          c.set_coefficient(mu, Mj1I.get_entry(lambda.c(),mu.c()));
    }
    else if ((lambda.k() >= DeltaRTmax(lambda.j())-2) && (lambda.k() <= DeltaRTmax(lambda.j()))) { // right boundary block involved
      int row = Mj1R.row_dimension()-number_of_components*(DeltaRTmax(lambda.j())-lambda.k()+1)+lambda.c();
      for (mu = SBasis::Index(lambda.j()-1,E_WAVELET,Nablamax(lambda.j()-1),number_of_components-1,this), i = Mj1R.column_dimension()-1; mu.k() == DeltaRTmax(lambda.j()-1); --mu, i--)
        c.set_coefficient(mu, Mj1R.get_entry(row, i));
      for(row = Mj1I.row_dimension()-number_of_components*(DeltaRTmax(lambda.j())-lambda.k()+1)+lambda.c(); mu.k() == Nablamax(lambda.j()-1)-1; --mu)
        c.set_coefficient(mu, Mj1I.get_entry(row,mu.c()));
      if (lambda.k() == DeltaRTmax(lambda.j())-2) {
        for(; mu.k() == Nablamax(lambda.j()-1)-2; --mu)
          c.set_coefficient(mu, Mj1I.get_entry(Mj1I.row_dimension()-2+lambda.c(),mu.c()));
      }
    }
    else { // only inner blocks involved
      dk = ((lambda.k()-DeltaLTmin())%2 == 0) ? 1 : 0;
      k_start = Nablamin() + (lambda.k()-DeltaLTmin()-1)/2;
      mu_end = SBasis::Index(lambda.j()-1,E_WAVELET,k_start+1+dk,number_of_components-1,this);
//      cout << k_start << mu_end << endl;
      for (mu = SBasis::Index(lambda.j()-1,E_WAVELET,k_start,0,this); mu <= mu_end; ++mu) {
//        cout << mu << (k_start-mu.k())*4+6+2*dk+lambda.c() << "  ";
        c.set_coefficient(mu, Mj1I.get_entry((k_start-mu.k())*4+6+2*dk+lambda.c(),mu.c()));
      }
    }
  }
  
  void
  SBasis::decompose_t_1(const SBasis::Index& lambda, const int jmin,
                        InfiniteVector<double, SBasis::Index>& c) const
  {
    assert(jmin >= j0());
    assert(lambda.j() >= jmin);
    
    InfiniteVector<double, SBasis::Index> cphi, ctmp;

    c.clear();

    if (lambda.e() == E_WAVELET) // wavelet
      c.set_coefficient(lambda, 1.0); // true wavelet coefficients don't have to be modified
    else { // generator
      cphi.set_coefficient(lambda, 1.0);
      for (int jnow = lambda.j(); jnow > jmin; jnow--) {
        ctmp.clear();
        for (InfiniteVector<double, SBasis::Index>::const_iterator it(cphi.begin()), itend(cphi.end());
             it != itend; ++it) {
          InfiniteVector<double, SBasis::Index> vtmp;
          decompose_t_1(it.index(), vtmp);
          ctmp.add(*it, vtmp); // ctmp += (*it) * vtmp
        }
        // now we have the refined coefficient set in ctmp, all indices have |\lambda| = jnow
        // write all generator indices into cphi, add wavelet indices to c
        cphi.clear();
        for (InfiniteVector<double, SBasis::Index>::const_iterator it(ctmp.begin()), itend(ctmp.end());
             it != itend; ++it) {
          if (it.index().e() == E_WAVELET)
            c.add(*it, ctmp);
          else
            cphi.add(*it, ctmp);
        }
      }
      // copy contents of cphi to c
      for (InfiniteVector<double, SBasis::Index>::const_iterator it(cphi.begin()), itend(cphi.end());
           it != itend; ++it)
        c.set_coefficient(it.index(), *it); // there shouldn't be entries in the generators present
    }
  }

  void
  SBasis::decompose_t(const InfiniteVector<double, SBasis::Index>& c, const int jmin,
                      InfiniteVector<double, SBasis::Index>& v) const
  {
    v.clear();
    InfiniteVector<double, SBasis::Index> vtmp;
    for (InfiniteVector<double, SBasis::Index>::const_iterator it(c.begin()), itend(c.end());
         it != itend; ++it) {
      decompose_t_1(it.index(), jmin, vtmp); // calls vtmp.clear() first
      v.add(*it, vtmp);
    }
  }


  /* RECONSTRUCT methods */  
  void
  SBasis::reconstruct_1(const SBasis::Index& lambda, InfiniteVector<double, SBasis::Index>& c) const
  {
    assert(lambda.e() == E_WAVELET);

    c.clear();

    SBasis::Index mu;
    int column;

    // build corresponding row of M_{j,1}
    if ( (lambda.k() >= Nablamin()) && (lambda.k() <= NablaLmax()) ) { // left boundary block of M_{j,1}
      column = number_of_components*(lambda.k()-Nablamin()) + lambda.c();
      for (mu = SBasis::Index(lambda.j()+1,E_GENERATOR,DeltaLmin(),0,this); mu.k() <= DeltaLmin()+Mj1L.row_dimension()/number_of_components-1; ++mu)
        c.set_coefficient(mu, Mj1L.get_entry(number_of_components*(mu.k()-DeltaLmin()) + mu.c(), column));
    }
    else if ( (lambda.k() >= NablaRmin(lambda.j())) && (lambda.k() <= Nablamax(lambda.j())) ) { // right boundary block of M_{j,1}
      column = number_of_components*(lambda.k()-NablaRmin(lambda.j())) + lambda.c();
      const SBasis::Index::translation_type k_start = DeltaRmax(lambda.j()+1)-Mj1R.row_dimension()/number_of_components+1;
      for (mu = SBasis::Index(lambda.j()+1,E_GENERATOR,k_start,0,this); mu <= last_generator(lambda.j()+1); ++mu)
        c.set_coefficient(mu, Mj1R.get_entry(number_of_components*(mu.k()-k_start) + mu.c(), column));
    }
    else { // inner block
      const SBasis::Index::translation_type k_start = DeltaLmin()+(lambda.k()-1)*2;
      const SBasis::Index mu_end(lambda.j()+1, E_GENERATOR, k_start+Mj1I.row_dimension()/number_of_components-1, number_of_components-1, this); // alltogether 5 non-zero k-indices
      for (mu = SBasis::Index(lambda.j()+1,E_GENERATOR,k_start,0,this); mu <= mu_end; ++mu)
        c.set_coefficient(mu, Mj1I.get_entry(number_of_components*(mu.k()-k_start) + mu.c(), lambda.c()));
    }
  }

  void
  SBasis::reconstruct_1(const SBasis::Index& lambda, const int j,
                        InfiniteVector<double, SBasis::Index>& c) const
  {
    c.clear();

    if (lambda.e() == E_GENERATOR) // it's already a generator
      refine_1(lambda, j, c);
    else { // it's a wavelet
      // write the wavelet in the generators
      // \Psi_j = M_{j,1}^T \Phi_{j+1}
      InfiniteVector<double, SBasis::Index> ctmp;
      reconstruct_1(lambda, ctmp);

      // now call the refine method for further refinement
      refine(ctmp, j, c);
    }
  }
    
  void
  SBasis::reconstruct(const InfiniteVector<double, SBasis::Index>& c, const int j,
                      InfiniteVector<double, SBasis::Index>& v) const
  {
    v.clear();
    for (InfiniteVector<double, SBasis::Index>::const_iterator it(c.begin()), itend(c.end()); it != itend; ++it) {
      InfiniteVector<double, SBasis::Index> vtmp;
      reconstruct_1(it.index(), j, vtmp);
      v.add(*it, vtmp);
    }
  }

  void
  SBasis::reconstruct_t_1(const SBasis::Index& lambda, InfiniteVector<double, SBasis::Index>& c) const
  {
    assert(lambda.e() == E_WAVELET);

    c.clear();

    // build corresponding row of \tilde M_{j,1} = G_{j,1}^T
    const SBasis::Index::translation_type k_start = DeltaLTmin() - 1 + (lambda.k()-Nablamin())*2;
    SBasis::Index mu(lambda.j()+1,E_GENERATOR,max(DeltaLTmin(), k_start),0,this);
    SBasis::Index mu_end(lambda.j()+1, E_GENERATOR, min(DeltaRTmax(lambda.j()+1), k_start+(int)(MTj1I.row_dimension()/number_of_components-1)), number_of_components-1, this);
    for (; mu <= mu_end; ++mu)
      c.set_coefficient(mu, MTj1I.get_entry((mu.k()-k_start)*number_of_components+mu.c(),lambda.c()));
  }

  void
  SBasis::reconstruct_t_1(const SBasis::Index& lambda, const int j,
                          InfiniteVector<double, SBasis::Index>& c) const
  {
    c.clear();

    if (lambda.e() == E_GENERATOR) // it's already a generator
      refine_t_1(lambda, j, c);
    else { // it's a wavelet
      // write the wavelet in the generators
      // \tilde\Psi_j = \tilde M_{j,1}^T \tilde\Phi_{j+1} = G_{j,1} \tilde\Phi_{j+1}
      InfiniteVector<double, SBasis::Index> ctmp;
      reconstruct_1(lambda, ctmp);

      // now call the refine method for further refinement
      refine_t(ctmp, j, c);
    }
  }

  void
  SBasis::reconstruct_t(const InfiniteVector<double, SBasis::Index>& c, const int j,
                        InfiniteVector<double, SBasis::Index>& v) const
  {
    v.clear();
    for (InfiniteVector<double, SBasis::Index>::const_iterator it(c.begin()), itend(c.end()); it != itend; ++it) {
      InfiniteVector<double, SBasis::Index> vtmp;
      reconstruct_t_1(it.index(), j, vtmp);
      v.add(*it, vtmp);
    }
  }


  /* REFINE methods */
  void
  SBasis::refine_1(const SBasis::Index& lambda, InfiniteVector<double, SBasis::Index>& c) const
  {
    assert(lambda.e() == E_GENERATOR);

    SBasis::Index mu;

    c.clear();

    // we only have inner functions, so just apply the refinement equation
    // \Phi_{(j,k)} = \frac{1}{\sqrt{2}} \sum_{\ell=2k-1}^{2k+1} \mat{A}_{\ell-2k} \Phi_{(j+1,\ell)}
    SBasis::Index mu_end = SBasis::Index(lambda.j()+1,E_GENERATOR,2*lambda.k()+1,number_of_components-1,this);
    for (mu = SBasis::Index(lambda.j()+1,E_GENERATOR,2*lambda.k()-1,0,this); mu <= mu_end; ++mu) {
      c.set_coefficient(mu, mask_primal.get_coefficient(mu.k()-2*lambda.k()).get_entry(lambda.c(),mu.c()));
    }
    c *= M_SQRT1_2;
  }

  void
  SBasis::refine_1(const SBasis::Index& lambda, const int j, InfiniteVector<double, SBasis::Index>& c) const
  {
    int jnow;
    InfiniteVector<double, SBasis::Index> ctmp;

    c.clear();
    c.set_coefficient(lambda, 1.0);

    for (jnow = lambda.j(); jnow < j; jnow++) {
      // refine from jnow to jnow+1
      ctmp.clear();
      for (InfiniteVector<double, SBasis::Index>::const_iterator it(c.begin()), itend(c.end()); it != itend; ++it) {
        InfiniteVector<double, SBasis::Index> vtmp;
        refine_1(it.index(), vtmp);
        ctmp.add(*it, vtmp); // ctmp += (*it) * vtmp
      }
      // now we have the refined coefficient set in ctmp
      // all indices have |\lambda| = jnow
      c = ctmp;
    }
  }

  void
  SBasis::refine(const InfiniteVector<double, SBasis::Index>& c, const int j,
                 InfiniteVector<double, SBasis::Index>& v) const
  {
    v.clear();
    for (InfiniteVector<double, SBasis::Index>::const_iterator it(c.begin()), itend(c.end()); it != itend; ++it) {
      InfiniteVector<double, SBasis::Index> vtmp;
      refine_1(it.index(), j, vtmp);
      v.add(*it, vtmp);
    }
  }

  void
  SBasis::refine_t_1(const SBasis::Index& lambda, InfiniteVector<double, SBasis::Index>& c) const
  {
    assert(lambda.e() == E_GENERATOR);

    SBasis::Index mu;

    c.clear();

    // is it a boundary function?
    if ((lambda.k() >= DeltaLTmin()) && (lambda.k() <= DeltaLTmax())) { // yes, left boundary
      for (mu = SBasis::Index(lambda.j()+1,E_GENERATOR,DeltaLTmin(),0,this); mu.k() < DeltaLTmin()+MLT.row_dimension()/number_of_components; ++mu) {
        c.set_coefficient(mu, MLT.get_entry((mu.k()-DeltaLmin())*number_of_components+mu.c(),lambda.c()));
      }
    }
    else if ((lambda.k() >= DeltaRTmin(lambda.j())) && (lambda.k() <= DeltaRTmax(lambda.j()))) { // yes, right boundary
      SBasis::Index::translation_type k_start = DeltaRTmax(lambda.j()+1)-MRT.row_dimension()/number_of_components+1;
      for (mu = SBasis::Index(lambda.j()+1,E_GENERATOR,k_start,0,this); mu <= last_generator(lambda.j()+1); ++mu) {
        c.set_coefficient(mu, MRT.get_entry((mu.k()-k_start)*number_of_components+mu.c(),lambda.c()));
      }
    }
    else { // no, inner function
      for (mu = SBasis::Index(lambda.j()+1,E_GENERATOR,2*lambda.k()-2,0,this); mu.k() <= 2*lambda.k()+2; ++mu) {
        c.set_coefficient(mu, mask_dual.get_coefficient(mu.k()-2*lambda.k()).get_entry(lambda.c(),mu.c()));
      }
    }
    c *= M_SQRT1_2;
  }

  void
  SBasis::refine_t_1(const SBasis::Index& lambda, const int j, InfiniteVector<double, SBasis::Index>& c) const
  {
    int jnow;
    InfiniteVector<double, SBasis::Index> ctmp;

    c.clear();
    c.set_coefficient(lambda, 1.0);

    for (jnow = lambda.j(); jnow < j; jnow++) {
      // refine from jnow to jnow+1
      ctmp.clear();
      for (InfiniteVector<double, SBasis::Index>::const_iterator it(c.begin()), itend(c.end()); it != itend; ++it) {
        InfiniteVector<double, SBasis::Index> vtmp;
        refine_t_1(it.index(), vtmp);
        ctmp.add(*it, vtmp); // ctmp += (*it) * vtmp
      }
      // now we have the refined coefficient set in ctmp
      // all indices have |\lambda| = jnow
      c = ctmp;
    }
  }

  void
  SBasis::refine_t(const InfiniteVector<double, SBasis::Index>& c, const int j,
                   InfiniteVector<double, SBasis::Index>& v) const
  {
    v.clear();
    for (InfiniteVector<double, SBasis::Index>::const_iterator it(c.begin()), itend(c.end()); it != itend; ++it) {
      InfiniteVector<double, SBasis::Index> vtmp;
      refine_t_1(it.index(), j, vtmp);
      v.add(*it, vtmp);
    }
  }

}
