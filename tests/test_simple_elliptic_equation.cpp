#include <map>
#include <fstream>
#include <iostream>
#include <time.h> 
#include <interval/ds_basis.h>
#include <interval/p_basis.h>
#include <interval/spline_basis.h>
#include <numerics/corner_singularity.h>
#include <simple_elliptic_equation.h>
#include <algebra/sparse_matrix.h>
#include <algebra/infinite_vector.h>
#include <numerics/iteratsolv.h>
#include <frame_evaluate.h>
#include <cube/cube_basis.h>
#include <cube/cube_index.h>
//#include <galerkin_frame_utils.h>
#include <galerkin/galerkin_utils.h>
#include <galerkin/cached_problem.h>
#include <frame_support.h>


using std::cout;
using std::endl;

using FrameTL::EvaluateFrame;
using FrameTL::SimpleEllipticEquation;
using FrameTL::AggregatedFrame;
using MathTL::EllipticBVP;
using MathTL::PoissonBVP;
using MathTL::ConstantFunction;
using MathTL::CornerSingularityRHS;
using MathTL::SparseMatrix;
using MathTL::InfiniteVector;
using WaveletTL::CubeBasis;
using WaveletTL::CubeIndex;

using namespace std;
using namespace FrameTL;
using namespace MathTL;
using namespace WaveletTL;

#define _FRAMETL_ADAPTIVE_COMPUTATION  0

int main()
{
  
  cout << "Testing class EllipticEquation..." << endl;
  
  const int DIM = 2;
  const int jmax = 6;

  const int d  = 2;
  const int dT = 2;

  //typedef DSBasis<2,2> Basis1D;

  typedef PBasis<d,dT> Basis1D;
  //typedef SplineBasis<d,dT,P_construction> Basis1D;

  typedef AggregatedFrame<Basis1D,2,2> Frame2D;
  typedef CubeBasis<Basis1D> Basis;
  typedef Frame2D::Index Index;

  EvaluateFrame<Basis1D,2,2> evalObj;

  //##############################  
  Matrix<double> A(DIM,DIM);
  A(0,0) = 2.;
  A(1,1) = 1.0;
  Point<2> b;
  b[0] = -1.;
  b[1] = -1.;
  AffineLinearMapping<2> affineP(A,b);

  Matrix<double> A2(DIM,DIM);
  A2(0,0) = 1.;
  A2(1,1) = 2.;
  Point<2> b2;
  b2[0] = -1.;
  b2[1] = -1.;
  AffineLinearMapping<2> affineP2(A2,b2);
  //##############################

  Array1D<Chart<DIM,DIM>* > charts(2);
  charts[0] = &affineP;
  charts[1] = &affineP2;
 
  SymmetricMatrix<bool> adj(2);
  adj(0,0) = 1;
  adj(1,1) = 1;
  adj(1,0) = 1;
  adj(0,1) = 1;
  
  //to specify primal boundary the conditions
  Array1D<FixedArray1D<int,2*DIM> > bc(2);

  //primal boundary conditions for first patch: all Dirichlet
  FixedArray1D<int,2*DIM> bound_1;
  bound_1[0] = 1;
  bound_1[1] = 1;
  bound_1[2] = 1;
  bound_1[3] = 1;//2

  bc[0] = bound_1;

  //primal boundary conditions for second patch: all Dirichlet
  FixedArray1D<int,2*DIM> bound_2;
  bound_2[0] = 1;
  bound_2[1] = 1;//2
  bound_2[2] = 1;
  bound_2[3] = 1;

  bc[1] = bound_2;

//to specify primal boundary the conditions
  Array1D<FixedArray1D<int,2*DIM> > bcT(2);

  //dual boundary conditions for first patch
  FixedArray1D<int,2*DIM> bound_3;
  bound_3[0] = 0;
  bound_3[1] = 0;
  bound_3[2] = 0;
  bound_3[3] = 0;

  bcT[0] = bound_3;

  //dual boundary conditions for second patch
  FixedArray1D<int,2*DIM> bound_4;
  bound_4[0] = 0;
  bound_4[1] = 0;
  bound_4[2] = 0;
  bound_4[3] = 0;
 
  bcT[1] = bound_4;

  Atlas<DIM,DIM> Lshaped(charts,adj);  
  cout << Lshaped << endl;

  //finally a frame can be constructed
  //AggregatedFrame<Basis1D, DIM, DIM> frame(&Lshaped, bc, bcT, jmax);
  AggregatedFrame<Basis1D, DIM, DIM> frame(&Lshaped, bc, jmax);

//   for (int i = 0; i < 480; i++) {
//     const Index* ind = frame.get_wavelet(i);
//     cout << (*ind) << endl;
//   }

  Vector<double> value(1);
  value[0] = 1;
  
  ConstantFunction<DIM> const_fun(value);
  Point<2> origin;
  origin[0] = 0.0;
  origin[1] = 0.0;

  CornerSingularityRHS singRhs(origin, 0.5, 1.5);
  CornerSingularity sing2D(origin, 0.5, 1.5);
  
  PoissonBVP<DIM> poisson(&singRhs);
  //PoissonBVP<DIM> poisson(&const_fun);

  // BiharmonicBVP<DIM> biahrmonic(&singRhs);

  SimpleEllipticEquation<Basis1D,DIM> discrete_poisson(&poisson, &frame, jmax);
  CachedProblem<SimpleEllipticEquation<Basis1D,DIM> > problem(&discrete_poisson, 5.0048, 1.0/0.01);

  double tmp = 0.0;
  int c = 0;
  //int d = 0;
  
  cout.precision(12);
  
  InfiniteVector<double,Frame2D::Index> rhs;
  Vector<double> v(225);
  
  FixedArray1D<bool,4> bcn;
  bcn[0] = bcn[1] = true;
  bcn[2] = bcn[3] = true;
  Basis basis(bcn);
  

  Point<2> x1;
  x1[0] = -1;
  x1[1] = -1;

  Point<2> x2;
  x2[0] = 1;
  x2[1] = 1;

//   Grid<2> grid(x1,x2,128);
//   SampledMapping<2> rhsOut (grid,singRhs);

//   std::ofstream ofs_si("si_rhs_out.m");
//   rhsOut.matlab_output(ofs_si);
//   ofs_si.close();


//   FrameIndex<Basis1D,2,2> index = FrameTL::first_generator<Basis1D,2,2,Frame2D>(&frame, frame.j0());
  
//   SampledMapping<2> wav_out = evalObj.evaluate(frame,index,1,5);

//   std::ofstream ofs("wav_out.m");
//   wav_out.matlab_output(ofs);
//   ofs.close();
  
//   Array1D<SampledMapping<2> > expansion_out = evalObj.evaluate(frame,rhs, 1, 5);
  
//   std::ofstream ofs2("expan_out.m");
//   expansion_out[0].matlab_output(ofs2);
//   ofs2.close();
  
//   std::ofstream ofs3("full_expan_out.m");
//   matlab_output(ofs3,expansion_out);
//   ofs3.close();
  
  //###############################################
  // testing correctness of routine for
  // computation of right hand side
  
  
//   cout << "++++++++++++++++++++++" << endl;
//   cout << rhs << endl;

//   Array1D<SampledMapping<2> > S = evalObj.evaluate(frame, rhs, false, 5);// expand in dual basis
//   std::ofstream ofs4("rhs_out.m");
//   matlab_output(ofs4,S);
//   ofs4.close();
  //###############################################   
  //############### 2D galerkin scheme test ##################
#if 1

  set<Index> Lambda;
  for (FrameIndex<Basis1D,2,2> lambda = FrameTL::first_generator<Basis1D,2,2,Frame2D>(&frame, frame.j0());
       lambda <= FrameTL::last_wavelet<Basis1D,2,2,Frame2D>(&frame, jmax); ++lambda) {
    Lambda.insert(lambda);
    //cout << lambda << endl;
  }
  
  cout << "setting up full right hand side..." << endl;
  Vector<double> rh;
  WaveletTL::setup_righthand_side(discrete_poisson, Lambda, rh);
  cout << rh << endl;
  cout << "setting up full stiffness matrix..." << endl;
  SparseMatrix<double> stiff;
  
  clock_t tstart, tend;
  double time;
  tstart = clock();

  WaveletTL::setup_stiffness_matrix(problem, Lambda, stiff);
  //WaveletTL::setup_stiffness_matrix(discrete_poisson, Lambda, stiff);
  //WaveletTL::setup_stiffness_matrix(problem, Lambda, stiff);

  tend = clock();
  time = (double)(tend-tstart)/CLOCKS_PER_SEC;
  cout << "  ... done, time needed: " << time << " seconds" << endl;

  stiff.matlab_output("stiff_2D_out", "stiff",1);  
  
  unsigned int iter= 0;
  Vector<double> x(Lambda.size()); x = 1;
  //double lmax = PowerIteration(stiff, x, 0.01, 1000, iter);
  double lmax = 1;
  cout << "lmax = " << lmax << endl;

  x = 1;
  //double lmin = InversePowerIteration(stiff, x, 0.01, 1000, iter);
  
  cout << "performing iterative scheme to solve projected problem..." << endl;
  Vector<double> xk(Lambda.size()), err(Lambda.size()); xk = 0;
  
 

  //CG(stiff, rh, xk, 1.0e-6, 100, iter);
  //cout << "CG iterations needed: "  << iter << endl;
  //Richardson(stiff, rh, xk, 2. / lmax - 0.01, 0.0001, 2000, iter);
  double alpha_n = 2. / lmax - 0.001;
  
  Vector<double> resid(xk.size());
  Vector<double> help(xk.size());
  for (int i = 0; i < 500;i++) {
    stiff.apply(xk,help);
    resid = rh - help;
    cout << "i = " << i << " " << sqrt(resid*resid) << endl;
    stiff.apply(resid,help);
    alpha_n = (resid * resid) * (1.0 / (resid * help));
    resid *= alpha_n;
    xk = xk + resid;
  }

  for (int i = 0; i < 450; i++) 
    for (int j = 0; j < 450; j++) {
      if (! (fabs(stiff.get_entry(i,j) -  stiff.get_entry(j,i)) < 1.0e-13)) {
	cout << stiff.get_entry(i,j) << endl;
	cout << stiff.get_entry(j,i) << endl;
	cout << "i = " << i << " j = " << j << endl;
	cout << "#######################" << endl;
	abort();
      }
    }


  cout << "performing output..." << endl;
  
  InfiniteVector<double,Frame2D::Index> u;
  unsigned int i = 0;
  for (set<Index>::const_iterator it = Lambda.begin(); it != Lambda.end(); ++it, ++i)
    u.set_coefficient(*it, xk[i]);
  
  u.scale(&discrete_poisson,-1);
  
  
  Array1D<SampledMapping<2> > U = evalObj.evaluate(frame, u, true, 6);//expand in primal basis
  
  std::ofstream ofs5("approx_solution_out.m");
  matlab_output(ofs5,U);
  ofs5.close();
  
//   cout << "computing L_2 error..." << endl;
//   double L2err = evalObj.L_2_error(frame, u, sing2D, 5, 0.0, 1.0);
//   cout << "...done L_2 error = " << L2err  << endl;
  

#if 0
  char filename1[50];
  u.clear();
  Lambda.clear();
  int l = 0;
  for (Index lambda = FrameTL::first_generator<Basis1D,2,2,Frame2D>(&frame, frame.j0());
       lambda <= FrameTL::last_wavelet<Basis1D,2,2,Frame2D>(&frame, frame.j0()); ++lambda) {

    if (l != 193) {
      l++;
      continue;
    }

    Index mu = FrameTL::first_generator<Basis1D,2,2,Frame2D>(&frame, frame.j0());
   
    
    cout << "result = " << discrete_poisson.a(lambda,mu) << endl;

    typedef WaveletTL::CubeBasis<Basis1D,DIM> CUBEBASIS;
    
    typedef CUBEBASIS::Index CubeIndex;
    
    CUBEBASIS::Support supp_lambda;
    
    WaveletTL::support<Basis1D,DIM>(*frame.bases()[lambda.p()], 
				    CubeIndex(lambda.j(),
					      lambda.e(),
					      lambda.k(),
					      frame.bases()[lambda.p()]),
				    supp_lambda);
    



    double dx = 1./(1 << supp_lambda.j);
//     cout << "l= " << l << " lambda:" << endl;
//     for (unsigned int i = 0; i < 2; i++) {
//       cout << "a_lambda = " << supp_lambda.a[i]*dx <<  " b= " << supp_lambda.b[i]*dx << " p = " << lambda.p() << endl;
//       cout << "a_mu = " << supp_mu.a[i]*dx <<  " b= " << supp_mu.b[i]*dx << " p = " << mu.p() << endl;
//     }    
    
//     if (l != 14) {
//       l++;
//       continue;
//     }

    cout << lambda << endl;
    cout << "##################" << endl;
    u.set_coefficient(lambda, 1);
    
    Array1D<SampledMapping<2> > U = evalObj.evaluate(frame, u, true, 7);//expand in primal basis
    sprintf(filename1, "%s%d%s", "wav_", l, ".m");
    
    
    std::ofstream ofs5(filename1);
    matlab_output(ofs5,U);
    ofs5.close();

    u.clear();
    ++l;

  }
#endif


  
  //cout << "lmin = " << lmin << endl;
  //########## testing runtime type information functionality #############
  //FrameTL::intersect_supports<Basis1D,2,2>(frame, index, index);
  //FrameIndex<Basis1D,2,2> index2 = FrameTL::last_generator<Basis1D,2,2,Frame2D>(&frame, frame.j0()); 
  //discrete_poisson.a(index,index2,3);
  //#######################################################################
#endif
  //################# end 2D galerkin scheme test ###################
  
  cout << "  ... done, time needed: " << time << " seconds" << endl;
  
#if 0
  MultiIndex<unsigned int, 2> e1;
  e1[0] = 0;
  e1[1] = 0;
  MultiIndex<int, 2> k1;
  k1[0] = 1;
  k1[1] = 1;
  
  MultiIndex<unsigned int, 2> e2;
  e2[0] = 0;
  e2[1] = 0;
  MultiIndex<int, 2> k2;
  k2[0] = 1;
  k2[1] = 1;
   
  unsigned int p1 = 1, p2 = 0;
  int j2 = 3;
   

  FrameIndex<Basis1D,2,2> la(&frame,j2,e1,p1,k1);
  FrameIndex<Basis1D,2,2> mu(&frame,j2,e2,p2,k2);
  cout << "val  " << discrete_poisson.a(la,mu,2) << endl;
#endif

  //    std::list<Index> intersecting;
  //    FrameTL::intersecting_wavelets<Basis1D,2,2>(frame, la, 4, false, intersecting);

  //    cout << intersecting.size() << endl;

  //    for (std::list<Index>::const_iterator  it = intersecting.begin();
  // 	it != intersecting.end(); ++it) {
  //      cout << *it << endl;
  //    }


   return 0;
}
