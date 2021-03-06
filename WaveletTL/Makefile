# +----------------------------------------------------------------+
# | Makefile for WaveletTL - Wavelet Template Library for C++      |
# |                                                                |
# | Copyright (c) 2002-2009                                        |
# | Thorsten Raasch, Manuel Werner                                 |
# +----------------------------------------------------------------+

all:: tests

# Currently, there are 5 sets of test programs, cf. tests/Makefile:
# set 1: stuff on R and R^d
# set 2: wavelet bases on the interval ([DS],[P],...)
# set 2a: periodic wavelet bases on the interval
# set 3: wavelet bases on general higher-dim. domains ((mapped) cube, tensor prod.)
# set 3a: wavelet bases on the ring domain
# set 3b: generic tensor product wavelet bases
# set 4: wavelet bases on the L-domain
# set 5: adaptive wavelet schemes for elliptic equations
# set 6: adaptive wavelet schemes for parabolic equations
# set 7: nonadaptive wavelet schemes for elliptic equations

tests::
	cd tests; $(MAKE)

tests1::
	cd tests; $(MAKE) tests1

tests2::
	cd tests; $(MAKE) tests2

tests2a::
	cd tests; $(MAKE) tests2a

tests3::
	cd tests; $(MAKE) tests3

tests3a::
	cd tests; $(MAKE) tests3a

tests3b::
	cd tests; $(MAKE) tests3b

tests4::
	cd tests; $(MAKE) tests4

tests5::
	cd tests; $(MAKE) tests5

tests6::
	cd tests; $(MAKE) tests6

tests7::
	cd tests; $(MAKE) tests7

doc::
	cd doc; $(MAKE)

clean::
	cd tests; $(MAKE) clean

veryclean:: clean
	cd tests; $(MAKE) veryclean

countlines::
	wc -l Rd/*.{h,cpp} interval/*.{h,cpp} galerkin/*.{h,cpp} adaptive/*.{h,cpp} generic/*.{h,cpp} cube/*.{h,cpp} Ldomain/*.{h,cpp} parabolic/*.{h,cpp} ring/*.{h,cpp} tests/*.cpp
