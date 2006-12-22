#include <iostream>
#include <set>

#include <utils/function.h>
#include <algebra/sparse_matrix.h>
#include <numerics/iteratsolv.h>
#include <Ldomain/ldomain_jl_basis.h>
#include <Ldomain/ldomain_jl_evaluate.h>
#include <Ldomain/ldomain_jl_expansion.h>

#define _WAVELETTL_GALERKINUTILS_VERBOSITY 0
#include <galerkin/ldomain_jl_gramian.h>
#include <galerkin/galerkin_utils.h>

#include "ldomain_solutions.h"

using namespace std;
using namespace MathTL;
using namespace WaveletTL;

int main()
{
  cout << "Testing LDomainJLGramian ..." << endl;

  typedef LDomainJLBasis Basis;
  typedef Basis::Index Index;

  Basis basis;
  
  const int solution = 1;
  Function<2> *uexact = 0, *f = 0;
  switch(solution) {
  case 1:
    uexact = new PolySolution();
    f = new PolyRHS();
    break;
  case 2:
    uexact = new EigenSolution();
    f = new EigenRHS();
    break;
  default:
    break;
  }

#if 0
  // temporary hack, choose f=1 just to see the inner products
  delete f;
  f = new ConstantFunction<2>(Vector<double>(1, "1"));
#endif

  const int jmax = 1;
  
  typedef LDomainJLGramian Problem;
  Problem problem(basis, InfiniteVector<double,Index>());

  InfiniteVector<double,Index> fcoeffs;
  expand(f, basis, true, jmax, fcoeffs);
  problem.set_rhs(fcoeffs);

//   cout << "- integrals of f against the primal wavelets:" << endl
//        << fcoeffs << endl;

  set<Index> Lambda;
  for (Index lambda = basis.first_generator(basis.j0());; ++lambda) {
    Lambda.insert(lambda);
//     if (lambda == basis.last_generator(basis.j0())) break;
    if (lambda == basis.last_wavelet(jmax)) break;
  }
  
  cout << "- set up Gramian matrix..." << endl;
  clock_t tstart, tend;
  double time;
  tstart = clock();

  SparseMatrix<double> A;
  setup_stiffness_matrix(problem, Lambda, A);
  tend = clock();
  time = (double)(tend-tstart)/CLOCKS_PER_SEC;
  cout << "  ... done, time needed: " << time << " seconds" << endl;
//   cout << "- Gramian matrix A=" << endl << A << endl;
#if 1
  A.matlab_output("LdomainJL_gramian", "G", 1);
#endif

#if 1
  cout << "- validate entries of the (unpreconditioned) Gramian matrix:" << endl;

  for (set<Index>::const_iterator itlambda = Lambda.begin(); itlambda != Lambda.end(); ++itlambda) {
    cout << "* checking row " << *itlambda << "..." << endl;
    for (set<Index>::const_iterator itmu = Lambda.begin(); itmu != Lambda.end(); ++itmu) {
      const int N = 2000;
      const double h = 1./N;

      double s = 0;

      FixedArray1D<Array1D<double>,2> lambdavalues, muvalues;
      FixedArray1D<Array1D<double>,2> knots;
      knots[0].resize(N+1);
      knots[1].resize(N+1);

      const Index& lambda = *itlambda;
      const Index& mu = *itmu;
      
      // patch Omega_0 = [-1,0]x[0,1]
      if (lambda.k()[0] <= 0 && lambda.k()[1] >= 0 && mu.k()[0] <= 0 && mu.k()[1] >= 0) {
	for (int k0 = 0; k0 < N; k0++)
	  knots[0][k0] = -1.0+k0*h;
	evaluate(0, lambda.j(), lambda.e()[0], lambda.c()[0], lambda.k()[0], knots[0], lambdavalues[0]);
	evaluate(0, mu.j(), mu.e()[0], mu.c()[0], mu.k()[0], knots[0], muvalues[0]);
	for (int k1 = 0; k1 < N; k1++)
	  knots[1][k1] = k1*h;
	evaluate(0, lambda.j(), lambda.e()[1], lambda.c()[1], lambda.k()[1], knots[1], lambdavalues[1]);
	evaluate(0, mu.j(), mu.e()[1], mu.c()[1], mu.k()[1], knots[1], muvalues[1]);
	for (int k0 = 0; k0 < N; k0++) {
	  for (int k1 = 1; k1 < N; k1++) {
	    s += lambdavalues[0][k0] * lambdavalues[1][k1]
	      * muvalues[0][k0] * muvalues[1][k1];
	  }
	}
      }
      // patch Omega_1 = [-1,0]x[-1,0]
      if (lambda.k()[0] <= 0 && lambda.k()[1] <= 0 && mu.k()[0] <= 0 && mu.k()[1] <= 0) {
	for (int k0 = 0; k0 < N; k0++)
	  knots[0][k0] = -1.0+k0*h;
	evaluate(0, lambda.j(), lambda.e()[0], lambda.c()[0], lambda.k()[0], knots[0], lambdavalues[0]);
	evaluate(0, mu.j(), mu.e()[0], mu.c()[0], mu.k()[0], knots[0], muvalues[0]);
	for (int k1 = 0; k1 < N; k1++)
	  knots[1][k1] = -1.0+k1*h;
	evaluate(0, lambda.j(), lambda.e()[1], lambda.c()[1], lambda.k()[1], knots[1], lambdavalues[1]);
	evaluate(0, mu.j(), mu.e()[1], mu.c()[1], mu.k()[1], knots[1], muvalues[1]);
	for (int k0 = 0; k0 < N; k0++) {
	  for (int k1 = 1; k1 < N; k1++) {
	    s += lambdavalues[0][k0] * lambdavalues[1][k1]
	      * muvalues[0][k0] * muvalues[1][k1];
	  }
	}
      }
      // patch Omega_2 = [0,1]x[-1,0]
      if (lambda.k()[0] >= 0 && lambda.k()[1] <= 0 && mu.k()[0] >= 0 && mu.k()[1] <= 0) {
	for (int k0 = 0; k0 < N; k0++)
	  knots[0][k0] = k0*h;
	evaluate(0, lambda.j(), lambda.e()[0], lambda.c()[0], lambda.k()[0], knots[0], lambdavalues[0]);
	evaluate(0, mu.j(), mu.e()[0], mu.c()[0], mu.k()[0], knots[0], muvalues[0]);
	for (int k1 = 0; k1 < N; k1++)
	  knots[1][k1] = -1.0+k1*h;
	evaluate(0, lambda.j(), lambda.e()[1], lambda.c()[1], lambda.k()[1], knots[1], lambdavalues[1]);
	evaluate(0, mu.j(), mu.e()[1], mu.c()[1], mu.k()[1], knots[1], muvalues[1]);
	for (int k0 = 0; k0 < N; k0++) {
	  for (int k1 = 1; k1 < N; k1++) {
	    s += lambdavalues[0][k0] * lambdavalues[1][k1]
	      * muvalues[0][k0] * muvalues[1][k1];
	  }
	}
      }
      
      const double alambdamu = problem.a(*itlambda,*itmu);
      const double alambdamuapprox = s*h*h;
      const double dev = fabs(alambdamu-alambdamuapprox);
      if (dev > 1e-6)
	cout << "lambda=" << *itlambda
	     << ", mu=" << *itmu << ": "
	     << "a(.,.)=" << alambdamu
	     << ", approx.=" << alambdamuapprox
	     << ", dev.=" << dev << endl;
    }
  }
#endif

  cout << "- set up right-hand side..." << endl;
  tstart = clock();
  Vector<double> b;
  setup_righthand_side(problem, Lambda, b);
  tend = clock();
  time = (double)(tend-tstart)/CLOCKS_PER_SEC;
  cout << "  ... done, time needed: " << time << " seconds" << endl;
//   cout << "- right hand side: " << b << endl;

  Vector<double> x(Lambda.size()), err(Lambda.size()); x = 0;
  unsigned int iterations;
  CG(A, b, x, 1e-15, 200, iterations);
  
//   cout << "- solution coefficients: " << x;
  cout << "- residual (infinity) norm: ";
  A.apply(x, err);
  err -= b;
  cout << linfty_norm(err) << endl;
  
#if 0
  cout << "- point values of the solution:" << endl;
  InfiniteVector<double,Index> u;
  unsigned int i = 0;
  for (set<Index>::const_iterator it = Lambda.begin(); it != Lambda.end(); ++it, ++i)
    u.set_coefficient(*it, x[i]);
  u.scale(&problem, -1);
  Array1D<SampledMapping<2> > s(evaluate(problem.basis(), u, 1<<5));
  std::ofstream u_Lambda_stream("u_lambda.m");
  matlab_output(u_Lambda_stream, s);
  u_Lambda_stream.close();
  cout << "  ... done, see file 'u_lambda.m'" << endl;
#endif

  if (uexact) delete uexact;
  if (f) delete f;

  return 0;
}
