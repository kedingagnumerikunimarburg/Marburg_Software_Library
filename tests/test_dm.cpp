#include <iostream>
#include <Rd/haar_mask.h>
#include <Rd/cdf_mask.h>
#include <Rd/dm_mask.h>
#include <Rd/refinable.h>

using namespace std;
using namespace WaveletTL;

int main()
{
  cout << "Testing the Dahmen/Micchelli mask ..." << endl;

//   cout << "- integral of two shifted Haar functions is the hat function:" << endl;
//   RefinableFunction<DMMask1<HaarMask, HaarMask> > dm;
//   cout << dm << endl;
  
//   cout << "- evaluate this at the integers:" << endl;
//   dm.evaluate<0u>(0, 0, -2, 2, 0).matlab_output(cout);

//   cout << "- integral of two Hat functions against each other:" << endl;
//   RefinableFunction<DMMask1<CDFMask_primal<2>, CDFMask_primal<2> > > dm2;
//   cout << dm2 << endl;
  
//   cout << "- evaluate this at the integers:" << endl;
//   dm2.evaluate<0u>(0, 0, -2, 2, 0).matlab_output(cout);

//   cout << "- DM multivariate mask for integrals of three Haar fcts.:" << endl;
//   MultivariateRefinableFunction<DMMask2<HaarMask, HaarMask, HaarMask>, 2> dm_multi;
//   cout << dm_multi << endl;
  
//   cout << "- evaluate this at the integers:" << endl;
//   cout << dm_multi.evaluate();

//   cout << "- DM multivariate mask for integrals of a Haar fct. and two translated hat fcts.:" << endl;
//   MultivariateRefinableFunction<DMMask2<HaarMask, CDFMask_primal<2>, CDFMask_primal<2> >, 2> dm_multi2;
//   cout << dm_multi2 << endl;
 
//   cout << "- evaluate this at the integers:" << endl;
//   cout << dm_multi2.evaluate();

  return 0;
}
