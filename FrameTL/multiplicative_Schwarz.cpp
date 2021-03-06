// implementation for steepest_descent.h

#include <cmath>
#include <set>
#include <utils/plot_tools.h>
#include <adaptive/apply.h>
#include <numerics/corner_singularity.h>
#include <frame_evaluate.h>
#include <poisson_1d_testcase.h>

using std::set;

namespace FrameTL
{
  /*!
  */
  template<class VALUE = double>
  class PolySolBiharmonic
    : public Function<1, VALUE>
  {
  public:
    PolySolBiharmonic() {};
    virtual ~PolySolBiharmonic() {};
    VALUE value(const Point<1>& p,
		const unsigned int component = 0) const
    {
      return 16*(p[0]*p[0])*(1-p[0])*(1-p[0]);
    }
  
    void vector_value(const Point<1> &p,
		      Vector<VALUE>& values) const { ; }
  
  };


  template <class PROBLEM>
  void GALERKIN (PROBLEM& P,
		 const set<typename PROBLEM::Index>& Lambda,
		 const InfiniteVector<double, typename PROBLEM::Index>& rhs,
		 Vector<double>& u)
  {
    typedef typename PROBLEM::Index Index;
    // setup stiffness matrix
    cout << "setting up full stiffness matrix..." << endl;
    SparseMatrix<double> A_Lambda;
    WaveletTL::setup_stiffness_matrix(P, Lambda, A_Lambda);

    cout << "setting up full right hand side..." << endl;
    Vector<double> F(Lambda.size());
    unsigned int id = 0;
    typename set<Index>::const_iterator it = Lambda.begin();
    for (; it != Lambda.end(); ++it, ++id) {
      F[id] = rhs.get_coefficient(*it);
    }

    cout << "size = " << F.size() << endl;
    unsigned int iterations = 0;
    CG(A_Lambda, F, u, 1.0e-15, 500, iterations);
    cout << "CG done!!!!" << " Needed " << iterations << " iterations" << endl;
  }

  
  // a very quick hack, only working for ery special case
  template <class PROBLEM>
  void REDUCE_REDUNDANCY (PROBLEM& P, const int i,
			  const InfiniteVector<double, typename PROBLEM::Index>& u,
			  InfiniteVector<double, typename PROBLEM::Index>& u_sparse)
  {

#if 1
    u_sparse.clear();
    typename InfiniteVector<double, typename PROBLEM::Index>::const_iterator it = u.begin();
    if (i==0) {
      Point<1> x(.8);
      for (; it != u.end(); ++it) {
	//if (it.index().p() == 1 && in_support(P.basis(), it.index(),x)) {
	if (it.index().p() == 1) {
	  u_sparse.set_coefficient(it.index(), *it);
	}
      }
    }
    else if (i==1) {
      Point<1> x(.2);
      for (; it != u.end(); ++it) {
	//if (it.index().p() == 0 && in_support(P.basis(), it.index(),x)) {
	if (it.index().p() == 0) {
	  u_sparse.set_coefficient(it.index(), *it);
	}
      }

    }
    //cout << u_sparse << endl;
    //u.clear();
    //u -= u_bak;
    //u.compress();
#endif
  }

  template <class PROBLEM>
  bool intersect_line1 (PROBLEM& P,
			const typename PROBLEM::Index& lambda)
  {
    typedef typename PROBLEM::WaveletBasis::Support SuppType;
    const SuppType* supp = &(P.basis().all_patch_supports[lambda.number()]);
    if ( (supp->a[0] < -0.7) && (-0.7 < supp->b[0]) && (supp->a[1] < 0.0) 
	 || 
	 (supp->a[1] < 0.0) && (0.0 < supp->b[1]) && (supp->b[0] > -0.7)
	 ) // L-shaped with smaller overlap
      //if ((supp->a[1] < 0.0) && (0.0 < supp->b[1]))  // usual L-shaped
      //if ((supp->a[0] < -0.3) && (-0.3 < supp->b[0]))
      return true;
    return false;
  }
  
  template <class PROBLEM>
  bool intersect_line2 (PROBLEM& P,
			const typename PROBLEM::Index& lambda)
  {
    typedef typename PROBLEM::WaveletBasis::Support SuppType;
    const SuppType* supp = &(P.basis().all_patch_supports[lambda.number()]);
    if ((supp->a[0] < 0.0) && (0.0 < supp->b[0]))
    //if ((supp->a[0] < 0.3) && (0.3 < supp->b[0]))
      return true;
    return false;

  }

  template <class PROBLEM>
  bool contact_with_patch2 (PROBLEM& P,
			    const typename PROBLEM::Index& lambda)
  {
    typedef typename PROBLEM::WaveletBasis::Support SuppType;
    const SuppType* supp = &(P.basis().all_patch_supports[lambda.number()]);
    //if ( leq(supp->b[0],0.3) )
    if ( leq(supp->b[0],0.0) )
      return true;
    return false;

  }


  template <class PROBLEM>
  void  multiplicative_Schwarz_SOLVE(const PROBLEM& P,  const double epsilon,
				     InfiniteVector<double, typename PROBLEM::Index>& u_epsilon_0,
				     InfiniteVector<double, typename PROBLEM::Index>& u_epsilon_1,
				     InfiniteVector<double, typename PROBLEM::Index>& u_epsilon)
  {
 //    double o_lap = 0.7;


//     typedef PBasis<3,3> Basis1D;
//     //typedef SplineBasis<2,2,P_construction> Basis1D;

//     typedef typename PROBLEM::WaveletBasis Frame;

//     const int jmax = 15;
//     typedef typename PROBLEM::Index Index;

//     double a_inv     = P.norm_Ainv();
//     double omega_i   = a_inv*P.F_norm();

//     InfiniteVector<double, Index> u_k, u_k_1_2, r, help, f, Av, r_exact, u_sparse;
    
//     map<double,double> log_10_residual_norms;
//     map<double,double> degrees_of_freedom;
//     map<double,double> asymptotic;
//     map<double,double> time_asymptotic;

//     bool exit = 0;
//     double time = 0;
//     clock_t tstart, tend;
//     tstart = clock();

//     double eta = 1.0e-15;

//     const int number_patches = P.basis().n_p();

//     unsigned int global_iterations = 0;
//     double tmp = 5.;

//     set<Index> I1;
//     set<Index> I2;
    
//     const int DIM = 1;

//     // setup index sets for blocks of stiffness matrix
//     for (Index lambda = FrameTL::first_generator<Basis1D,DIM,DIM,Frame>(&P.basis(), P.basis().j0());
// 	 lambda <= FrameTL::last_wavelet<Basis1D,DIM,DIM,Frame>(&P.basis(), jmax); ++lambda) {
//       if (lambda.p() == 0)
// 	I1.insert(lambda);
//       else if (lambda.p() == 1)
// 	I2.insert(lambda);
//     }
    
//     // setting up the four blocks of the stiffnes matrix
//     SparseMatrix<double> A11;
//     SparseMatrix<double> A12;
//     SparseMatrix<double> A21;
//     SparseMatrix<double> A22;
//     cout << "setting up A11... " << endl;
//     WaveletTL::setup_stiffness_matrix(P, I1, A11);
//     cout << "setting up A12... " << endl;
//     WaveletTL::setup_stiffness_matrix(P, I1, I2, A12);
//     //cout << A12 << endl;
//     cout << "setting up A21... " << endl;
//     WaveletTL::setup_stiffness_matrix(P, I2, I1, A21);
//     //cout << A21 << endl;
//     cout << "setting up A22... " << endl;
//     WaveletTL::setup_stiffness_matrix(P, I2, A22);

//     Vector<double> u_1(I1.size());
//     Vector<double> u_2(I2.size());

//     Vector<double> u_1_bak(I1.size());
//     Vector<double> u_2_bak(I2.size());

//     Vector<double> help_1(I1.size());
//     Vector<double> help_2(I2.size());

//     // setup full local right hand sides
//     Vector<double> f_1(I1.size());
//     Vector<double> f_2(I2.size());
//     cout << "setting up f1... " << endl;
//     WaveletTL::setup_righthand_side(P, I1, f_1);
//     cout << "setting up f2... " << endl;
//     WaveletTL::setup_righthand_side(P, I2, f_2);

    
//     double res_norm = 25;
//     while ( sqrt(res_norm) > 1.0e-10 && global_iterations < 100) {
//       cout << "##### global iteration = " << global_iterations << endl;
//       help_1 = 0;
//       help_2 = 0;
//       // perform the two half steps explicitely
//       unsigned int iterations = 0;
      
// #if 1 // 1D
//       // sparsen u_2
//       int k = 0;
//       Point<1> x(o_lap);
//       for (typename set<Index>::const_iterator it = I2.begin(); it != I2.end(); it++, k++) {
// 	if (! in_support(P.basis(), *it, x)) {
// 	  u_2[k] = 0;
// 	}
//       }
// #endif
// #if 0 // 2D
//       // sparsen u_2
//       int k = 0;
//       for (typename set<Index>::const_iterator it = I2.begin(); it != I2.end(); it++, k++) {
// 	if (! intersect_line1 (P,*it) ) {
// 	  u_2[k] = 0;
// 	}
//       }
// #endif

//       //cout << "sparse u2 " << u_2 << endl;
//       A12.apply(u_2,help_1);
//       CG(A11, (f_1 - help_1), u_1, 1.0e-15, 500, iterations); // u_1 = A11^-1 * (f_1 - A12*u_2);
//       cout << "needed " << iterations << " in CG" << endl;
//       //cout << "u1 after first partial step" << u_1 << endl;

// #if 1 // 1D
//       // backup u_1 now
//       u_1_bak = u_1;
      
//       // sparsen u_1
//       k = 0;
//       Point<1> y(1-o_lap);
//       for (typename set<Index>::const_iterator it = I1.begin(); it != I1.end(); it++, k++) {
// 	if (! in_support(P.basis(), *it, y)) {
// 	  u_1[k] = 0;
// 	}
//       }
//       //cout << "sparse u1 " << u_1 << endl;
// #endif      
// #if 0 // 2D
//       // backup u_1 now
//       u_1_bak = u_1;
      
//       // sparsen u_1
//       k = 0;
//       for (typename set<Index>::const_iterator it = I1.begin(); it != I1.end(); it++, k++) {
// 	if (! intersect_line2(P,*it) ) {
// 	  u_1[k] = 0;
// 	}
//       }
//       //cout << "sparse u1 " << u_1 << endl;
// #endif      


//       A21.apply(u_1,help_2);
//       CG(A22, (f_2 - help_2), u_2, 1.0e-15, 500, iterations); // u_2 = A22^-1 * (f_2 - A21*u_1);
//       cout << "needed " << iterations << " in CG" << endl;
//       //cout << "u2 after first partial step" << u_2 << endl;

// #if 1 //1D
//       // setup the current global approximation
//       // we have to remove those coefficients of u_1_bak that are contained in the closure of the second patch
//       typedef typename PROBLEM::WaveletBasis::Support SuppType;
//       k = 0;
//       for (typename set<Index>::const_iterator it = I1.begin(); it != I1.end(); it++, k++) {
// 	const SuppType* supp = &(P.basis().all_patch_supports[(*it).number()]);
	
// 	//cout << "#### " << supp->a[0] << " " << leq(0.3, supp->a[0]) << endl;
// 	if (leq(1-o_lap, supp->a[0]) ) {
// 	//if (! leq(supp->b[0], 0.2) ) {
// 	  u_1_bak[k] = 0;
// 	}
//       }
//       //cout << u_1_bak << endl;

//       u_1 = u_1_bak;
// #endif
// #if 0 //2D
//       // setup the current global approximation
//       // we have to remove those coefficients of u_1_bak that are contained in the closure of the second patch
//       typedef typename PROBLEM::WaveletBasis::Support SuppType;
//       k = 0;
//       for (typename set<Index>::const_iterator it = I1.begin(); it != I1.end(); it++, k++) {
// 	const SuppType* supp = &(P.basis().all_patch_supports[(*it).number()]);
// 	if (contact_with_patch2(P,*it) ) {
// 	  u_1_bak[k] = 0;
// 	}
//       }
//       //cout << u_1_bak << endl;

//       u_1 = u_1_bak;
// #endif



//       // compute residual norm:
//       A11.apply(u_1, help_1);
//       A12.apply(u_2, help_2);

//       res_norm = l2_norm_sqr(f_1-(help_1+help_2));

//       A21.apply(u_1, help_1);
//       A22.apply(u_2, help_2);
      
//       res_norm += l2_norm_sqr(f_2-(help_1+help_2));

//       cout << "residual norm = " << sqrt(res_norm) << " after iteration " << global_iterations << endl;
//       global_iterations++;
//     }

//     int id = 0;
//     for (typename set<Index>::const_iterator it = I1.begin(); it != I1.end(); it++, id++) {
//       if (u_1[id] != 0) {
// 	//cout << "index1 = " << (*it).number() << " " << u_1[id] << endl;
// 	u_epsilon_0.set_coefficient(*it, u_1[id]);
//       }
//     }

//     id = 0;
//     for (typename set<Index>::const_iterator it = I2.begin(); it != I2.end(); it++, id++) {
//       //cout << *it << endl;
//       if (u_2[id] != 0) {
// 	//cout << "index2 = " << (*it).number() << " " << u_2[id] << endl;
// 	u_epsilon_1.set_coefficient(*it, u_2[id]);
//       }
//     }

//     u_epsilon = u_epsilon_0 + u_epsilon_1;


//     //cout << "degrees_of_freedom = " << u_epsilon.size() << endl;

// //     SparseMatrix<double> A;
// //     set<Index> Lambda_sparse;
// //     u_epsilon.support(Lambda_sparse);
// //     WaveletTL::setup_stiffness_matrix(P, Lambda_sparse, A);
// //     A.matlab_output("stiff_sparse_1D_out", "stiff_sparse",1);

  }


}
