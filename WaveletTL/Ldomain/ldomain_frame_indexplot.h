// -*- c++ -*-

// +--------------------------------------------------------------------+
// | This file is part of WaveletTL - the Wavelet Template Library      |
// |                                                                    |
// | Copyright (c) 2002-2009                                            |
// | Thorsten Raasch, Manuel Werner, Ulrich Friedrich                   |
// +--------------------------------------------------------------------+

#ifndef _WAVELETTL_LDOMAIN_FRAME_INDEXPLOT_H
#define	_WAVELETTL_LDOMAIN_FRAME_INDEXPLOT_H

#include <iostream>
#include <Ldomain/ldomain_frame_index.h>
#include <algebra/infinite_vector.h>
//#include <vector>

using MathTL::InfiniteVector;
using std::cout;
using std::endl;

namespace WaveletTL
{
  /*
    Matlab plot of a given finite wavelet coefficient array
    w.r.t. a wavelet frame on the square as modeled by tframe.
    Only indices with 1-norm less or equal than ||j0()||_1+range will be plotted.
    Method works for DIM=2, but the code is already somewhat generalized for
    other dimensions.

    Since the color value of the rectangles will actually be
    log10(|c_lambda|/||c||_infty), you have to specify a range 10^a...10^0
    to which these values shall be clipped.
    You can choose a Matlab colormap and toggle the boxes and a colorbar on or off.
   * 
   *The L-shaped plot is realized via subplots. 
  */
  template <class LDOMAINFRAME>
  void plot_indices_ldomain(const LDOMAINFRAME* frame,
                     const InfiniteVector<double, typename LDOMAINFRAME::Index>& coeffs,
		     std::ostream& os,
                     const typename LDOMAINFRAME::Index::polynomial_type p = typename LDOMAINFRAME::Index::polynomial_type(),
                     const typename LDOMAINFRAME::Index::level_type j = typename LDOMAINFRAME::Index::level_type(),
                     const typename LDOMAINFRAME::Index::type_type e = typename LDOMAINFRAME::Index::type_type(),
		     const char* colormap = "cool",
		     bool boxed = false,
		     bool colorbar = true,
		     const double lowerclim = -6);
  
  //new compact plot
  template <class LDOMAINFRAME>
  void plot_indices_ldomain2(const LDOMAINFRAME* frame,
                     const InfiniteVector<double, typename LDOMAINFRAME::Index>& coeffs,
		     std::ostream& os,
                     const double threshold=1e-15);
}

// include implementation
#include <Ldomain/ldomain_frame_indexplot.cpp>

#endif	/* _WAVELETTL_LDOMAIN_FRAME_INDEXPLOT_H */

