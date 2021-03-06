// implementation for duv.h

#include <adaptive/apply.h>

namespace WaveletTL
{
  template <class PROBLEM>
  void DUV_SOLVE_SD(const PROBLEM& P, const double nu, const double epsilon,
		    InfiniteVector<double, typename PROBLEM::WaveletBasis::Index>& u_epsilon, const int jmax)
  {
      
      typedef typename PROBLEM::WaveletBasis::Index Index;

    // compute spectral norm rho
    const double cond_A = P.norm_A() * P.norm_Ainv();
    const double rho = (cond_A - 1.0) / (cond_A + 1.0);
    cout << "DUV_SOLVE_SD: rho=" << rho << endl;
    
    // desired error reduction factor theta < 1/3
    const double theta = 2.0/7.0;
    cout << "DUV_SOLVE_SD: theta=" << theta << endl;

    // compute minimal K such that 3*rho^K < theta
    const int K = (int) ceil(log10(theta/3.0) / log10(rho));
    cout << "DUV_SOLVE_SD: K=" << K << endl;
    
    u_epsilon.clear();

    double epsilon_k = nu;
    InfiniteVector<double,Index> f, rj, v, Av, Arj;
    while(epsilon_k > epsilon) {
      cout << "DUV_SOLVE_SD: epsilon_k=" << epsilon_k << endl;
      v = u_epsilon;
      for (int j = 0; j < K; j++) {
// 	cout << "size of current iterand j=" << j << ": " << v.size() << endl;
	const double eta = pow(rho, (double)j) * epsilon_k;
	P.RHS(eta, f);
// 	cout << "DUV_SOLVE_SD: f=" << endl << f << endl;
	APPLY_COARSE(P, v, eta, Av, 1.0, jmax);
// 	cout << "DUV_SOLVE_SD: Av=" << endl << Av << endl;
	rj = f - Av;
// 	cout << "DUV_SOLVE_SD: rj=" << endl << rj << endl;
	double residual_error = l2_norm(rj);
 	cout << "DUV_SOLVE_SD: epsilon_k=" << epsilon_k
	     << ", inner iteration j=" << j << " (" << K-1
	     << "), current residual error: " << residual_error << endl;
        cout << "DUV:: v.size() = " << v.size() << endl;
	if (residual_error < epsilon) break;

	APPLY_COARSE(P, rj, eta/5.0, Arj, 1.0, jmax);
// 	cout << "rj * rj = " << rj * rj << endl;
//  	cout << "DUV_SOLVE: rj * Arj = " << rj * Arj << endl;
	
	if (rj * Arj == 0)
	  {
	    cout << "DUV_SOLVE_SD: rj * Arj == 0!!!" << endl;
	    cout << "rj=" << endl << rj << endl << "Arj=" << endl << Arj << endl;
	    abort();
	  }

	const double alphaj = (rj * rj) / (rj * Arj);
// 	cout << "descent parameter alphaj=" << alphaj << endl;
	v += alphaj * rj;
      }
      v.COARSE(2.0*epsilon_k/5.0, u_epsilon);
//       u_epsilon = v;
      cout << "DUV:: u.size() = " << u_epsilon.size() << endl;
      epsilon_k /= 2.0;
    }
  }
  
  
  
  template <class PROBLEM>
  void DUV_QUARKLET_SOLVE_SD(const PROBLEM& P, const double nu, const double epsilon,
		    InfiniteVector<double, typename PROBLEM::QuarkletFrame::Index>& u_epsilon, CompressionStrategy strategy, const int pmax, 
                    const int jmax, const double a, const double b)
  {
    typedef typename PROBLEM::QuarkletFrame::Index Index;

    // compute spectral norm rho
    const double cond_A = P.norm_A() * P.norm_Ainv();
    const double rho = (cond_A - 1.0) / (cond_A + 1.0);
    cout << "DUV_SOLVE_SD: rho=" << rho << endl;
    
    // desired error reduction factor theta < 1/3
    const double theta = 2.0/7.0;
    cout << "DUV_SOLVE_SD: theta=" << theta << endl;

    // compute minimal K such that 3*rho^K < theta
    const int K = (int) ceil(log10(theta/3.0) / log10(rho));
    cout << "DUV_SOLVE_SD: K=" << K << endl;
    
    u_epsilon.clear();

    double epsilon_k = nu;
    InfiniteVector<double,Index> f, rj, v, Av, Arj;
    while(epsilon_k > epsilon) {
      cout << "DUV_SOLVE_SD: epsilon_k=" << epsilon_k << endl;
      v = u_epsilon;
      for (int j = 0; j < K; j++) {
// 	cout << "size of current iterand j=" << j << ": " << v.size() << endl;
	const double eta = pow(rho, (double)j) * epsilon_k;
	P.RHS(eta, f);
// 	cout << "DUV_SOLVE_SD: f=" << endl << f << endl;
	APPLY_QUARKLET_COARSE(P, v, eta, Av, jmax, strategy, pmax, a, b, 1.0);
//        APPLY_COARSE(P, v, eta, Av, 1.0, jmax, strategy);
// 	cout << "DUV_SOLVE_SD: Av=" << endl << Av << endl;
	rj = f - Av;
// 	cout << "DUV_SOLVE_SD: rj=" << endl << rj << endl;
	double residual_error = l2_norm(rj);
 	cout << "DUV_SOLVE_SD: epsilon_k=" << epsilon_k
	     << ", inner iteration j=" << j << " (" << K-1
	     << "), current residual error: " << residual_error << endl;
        cout << "DUV:: v.size() = " << v.size() << endl;
	if (residual_error < epsilon) break;

	APPLY_QUARKLET_COARSE(P, rj, eta/5.0, Arj, jmax, strategy, pmax, a, b, 1.0);
//        APPLY_COARSE(P, rj, eta/5.0, Arj, 1.0, jmax, strategy);
// 	cout << "rj * rj = " << rj * rj << endl;
//  	cout << "DUV_SOLVE: rj * Arj = " << rj * Arj << endl;
	
	if (rj * Arj == 0)
	  {
	    cout << "DUV_SOLVE_SD: rj * Arj == 0!!!" << endl;
	    cout << "rj=" << endl << rj << endl << "Arj=" << endl << Arj << endl;
	    abort();
	  }

	const double alphaj = (rj * rj) / (rj * Arj);
// 	cout << "descent parameter alphaj=" << alphaj << endl;
	v += alphaj * rj;
      }
      v.COARSE(2.0*epsilon_k/5.0, u_epsilon);
//       u_epsilon = v;
      cout << "DUV:: u.size() = " << u_epsilon.size() << endl;
      epsilon_k /= 2.0;
    }
  }
}
