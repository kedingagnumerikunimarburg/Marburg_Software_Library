/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <iostream>

#include <interval/p_basis.h>
#include <interval/ds_basis.h>
#include <utils/fixed_array1d.h>
#include <cube/tbasis.h>
#include <cube/tbasis_index.h>
#include <utils/multiindex.h>
#include <numerics/bvp.h>
#include <galerkin/tbasis_equation.h>
#include <galerkin/cached_tproblem.h>
#include <adaptive/cdd2.h>
#include <cube/tbasis_evaluate.h>
#include <interval/pq_frame.h>
#include <cube/tframe.h>

#include "TestFunctions2d.h"

#define NONADAPTIVE
#undef ADAPTIVE

using namespace std;
using namespace WaveletTL;

using MathTL::FixedArray1D;

//test functions and derivatives
// 1: f(x,y)=x(1-x)y(1-y), Laplace(x,y)=2x(1-x)+2y(1-y)
template <unsigned int N>
class mySolution
  : public Function<2,double>
{
public:
  virtual ~mySolution() {};
  double value(const Point<2>& p, const unsigned int component = 0) const {
    switch(N) {
    case 1:
      return p[0]*(1-p[0])*p[1]*(1-p[1]);
      break;
    case 2:
      return sin(M_PI*p[0])*sin(M_PI*p[1]);
      break;
    case 3:
      return
	(p[0]<=0.5 ? 4*p[0]*p[0]*(3-4*p[0]) : (2-2*p[0])*(2-2*p[0])*(4*p[0]-1))
	* (p[1]<=0.5 ? 4*p[1]*p[1]*(3-4*p[1]) : (2-2*p[1])*(2-2*p[1])*(4*p[1]-1));
      break;
    default:
      return 0;
      break;
    }
  }
  void vector_value(const Point<2>& p, Vector<double>& values) const {
    values[0] = value(p);
  }
};

template <unsigned int N>
class myRHS
  : public Function<2,double>
{
public:
  virtual ~myRHS() {};
  double value(const Point<2>& p, const unsigned int component = 0) const {
    switch(N) {
    case 1:
      return 2*(p[0]*(1-p[0])+p[1]*(1-p[1]));
      break;
    case 2:
      return 2*M_PI*M_PI*sin(M_PI*p[0])*sin(M_PI*p[1]);
      break;
    case 3:
      return
	-((p[0]<=0.5 ? 24-96*p[0] : 96*p[0]-72)
	  * (p[1]<=0.5 ? 4*p[1]*p[1]*(3-4*p[1]) : (2-2*p[1])*(2-2*p[1])*(4*p[1]-1))
	  +(p[0]<=0.5 ? 4*p[0]*p[0]*(3-4*p[0]) : (2-2*p[0])*(2-2*p[0])*(4*p[0]-1))
	  * (p[1]<=0.5 ? 24-96*p[1] : 96*p[1]-72));
      break;
    default:
      return 1;
      break;
    }
  }
  void vector_value(const Point<2>& p, Vector<double>& values) const {
    values[0] = value(p);
  }
  string toString() const
        {
            switch(N)
            {
                case 1:
                    return "L(x,y)=2x(1-x)+2y(1-y)";
                    break;
                case 2:
                    return "";
                    break;
                case 3:
                    return "1";
                    break;
                case 4:
                    return "";
                    break;
                   
                default:
                    return "TestProblem: N not defined.";
                    break;
            }
        }
};

int main()
{
    cout << "Testing tbasis adaptive" << endl;
    const int d  = 2;
    const int dT = 2;
    const unsigned int dim = 2; const int levelrange(6);
    const int jmax1=4;
    MultiIndex<int, dim> jmax;
    jmax[0]=jmax1 ,jmax[1]=jmax1;
    typedef PBasis<d,dT> Basis1d;
    typedef Basis1d::Index Index1d;
    typedef TensorBasis<Basis1d,dim> Basis;
    typedef Basis::Index Index;
    typedef PQFrame<d,dT> Frame1d;
    typedef TensorFrame<Frame1d,dim> Frame;
    
    FixedArray1D<int,2*dim> s;          //set order of boundary conditions
    s[0]=0, s[1]=0; s[2]=0, s[3]=0; 
    //cout << s << endl;
    FixedArray1D<bool, 2*dim> bc;
    bc[0]=true, bc[1]=true, bc[2]=true, bc[3]=true;
    
    //Basis1d basis1d(true,true);
    //basis1d.set_jmax(jmax1);
    
    //Basis basis(bc); 
    //basis.set_jmax(jmax);
    
    mySolution<1> solution1;
    myRHS<1> rhs1;
    //Function2 function2;
    
    PoissonBVP<dim> poisson1(&rhs1);
    cout << "hallo" << endl;
    TensorEquation<Basis1d,dim,Basis> eq(&poisson1, bc);
    //TensorEquation<Frame1d,dim,Frame> feq(&poisson1, bc);
    cout << eq.basis().degrees_of_freedom() << endl;
    //eq.basis().set_jmax(10);
    //eq.set_jmax(2*jmax1);
    cout << eq.basis().degrees_of_freedom() << endl;
    
#if 0
    //plot stiffness matrix index version 
    int n=eq.basis().degrees_of_freedom();
    Matrix<double> L(n,n);
    int i=0;
    for (Index ind1 = eq.basis().first_generator(), itend = eq.basis().last_wavelet(2*jmax1); ind1 <= itend; ++ind1, i++){
        int j=0;
        for (Index ind2 = eq.basis().first_generator(), itend = eq.basis().last_wavelet(2*jmax1); ind2 <= itend; ++ind2, j++){
            //Index ind1=*it1;
            //Index ind2=*it2;
            L(i,j)=eq.a(ind2,ind1)/(eq.D(ind1)*eq.D(ind2));
        }     
    }
    L.matlab_output("L", "L",0);
    cout << "plot stiffness matrix done" << endl;
#endif
  
#ifdef NONADAPTIVE
    set<Index> Lambda;  //setup index set
    int zaehler=0;
    for (Index lambda=eq.basis().first_generator(), itend=eq.basis().last_wavelet(6); lambda <= itend; ++lambda)
    {
        Lambda.insert(lambda);
        //cout << lambda << endl;
        zaehler++;
    }
    cout << zaehler << endl;
    
    SparseMatrix<double> A;
    setup_stiffness_matrix(eq,Lambda,A); 
    cout << "setup stiffness matrix done" << endl;
    Vector<double> F;
    setup_righthand_side(eq, Lambda, F);
    
    Vector<double> x(Lambda.size());
    x =0;
    unsigned int iterations;
    const int maxiterations = 99;
    CG(A,F,x,1e-8,maxiterations,iterations); 
    cout << "iterative solution computed" << endl;
    
    //plot solution
    InfiniteVector<double,Index> u,v;
    unsigned int i2 = 0;
    for (set<Index>::const_iterator it = Lambda.begin(); it != Lambda.end(); ++it, ++i2){
        u.set_coefficient(*it, x[i2]);
    }
    u.scale(&eq, -1);
    SampledMapping<2> sm1(evaluate(eq.basis(), u , true, 6));
    std::ofstream stream1("solution_na.m");
    sm1.matlab_output(stream1);
    stream1.close();
    
    //alternative plot solution
    int N=100;
    double h = 1e-2;
    Matrix<double> u_values(N+1,N+1);
    for (unsigned int i = 0; i <= N; i++) {
        cout << i << endl;
        for(unsigned int j=0; j<=N; j++) {
            Point<2> point(i*h,j*h);
            int id = 0;
            for (set<Index>::const_iterator it(Lambda.begin()); it != Lambda.end(); ++it, ++id) 
                u_values(i,j) += x[id] * eq.basis().evaluate(0, *it, point)*1./eq.D(*it);
        }
    }
    u_values.matlab_output("solution_na_alt","solution_na_alt",0);      //richtiger output für quarklets
#endif
#ifdef ADAPTIVE
    CachedTProblem<TensorEquation<Basis1d,dim,Basis> > cproblem1(&eq);
    InfiniteVector<double, Index> F_eta;
    cproblem1.RHS(1e-6, F_eta);
    const double nu = cproblem1.norm_Ainv() * l2_norm(F_eta);
    double epsilon = 1e-3;
    InfiniteVector<double, Index> u_epsilon, v;
    //CDD2_SOLVE(cproblem1, nu, epsilon, u_epsilon, 6);
    //APPLY(cproblem1, u_epsilon, 1e-9, v, 2*jmax1, CDD1);
#endif
    
    
    return 0;
}
