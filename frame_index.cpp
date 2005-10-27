// implementation for frame_index.h

#include <math.h>
#include <cmath>

 namespace FrameTL
 {

   template <class IBASIS, unsigned int DIM_d, unsigned int DIM_m>
   FrameIndex<IBASIS, DIM_d, DIM_m>::FrameIndex(const AggregatedFrame<IBASIS,DIM_d,DIM_m>* frame)
     : frame_(frame)
   {
     if (frame_ == 0) {
       j_ = 0; // invalid (e and k are initialized by zero automatically)
       p_ = 0;
     } else {
       j_ = frame_->j0(); // coarsest level;
       // e_ is zero by default: generator
       p_ = 0;
       for (unsigned int i = 0; i < DIM_d; i++)
	 k_[i] = WaveletTL::first_generator<IBASIS>(frame_->bases()[0]->bases()[i], j_).k();
     }
   }

   template <class IBASIS, unsigned int DIM_d, unsigned int DIM_m>
   FrameIndex<IBASIS, DIM_d, DIM_m>::FrameIndex(const FrameIndex& ind)
     : frame_(ind.frame()), j_(ind.j()), e_(ind.e()), p_(ind.p()), k_(ind.k())
   {
   }

   template <class IBASIS, unsigned int DIM_d, unsigned int DIM_m>
   FrameIndex<IBASIS, DIM_d, DIM_m>::FrameIndex(const AggregatedFrame<IBASIS,DIM_d,DIM_m>* frame,
						const CubeIndex<IBASIS,DIM_d>& c,
						const int p)
     : frame_(frame), j_(c.j()), e_(c.e()), p_(p), k_(c.k())
   {
   }

   template <class IBASIS, unsigned int DIM_d, unsigned int DIM_m>
   FrameIndex<IBASIS, DIM_d, DIM_m>::FrameIndex(const AggregatedFrame<IBASIS,DIM_d,DIM_m>* frame,
				      const int j,
				      const type_type& e,
				      const unsigned int p,
				      const translation_type& k)

     : frame_(frame), j_(j), e_(e), p_(p), k_(k)
   {
   }

   template <class IBASIS, unsigned int DIM_d, unsigned int DIM_m>
   inline
   bool
   FrameIndex<IBASIS, DIM_d, DIM_m>::operator == (const FrameIndex& lambda) const
   {
     return (j_ == lambda.j() &&
	     e_ == lambda.e() &&
	     p_ == lambda.p() &&
	     k_ == lambda.k());
   }

   template <class IBASIS, unsigned int DIM_d, unsigned int DIM_m>
   inline
   FrameIndex<IBASIS, DIM_d, DIM_m>&
   FrameIndex<IBASIS, DIM_d, DIM_m>::operator ++ ()
   {
     const unsigned int num_patches = (frame_->bases()).size();
     assert(0 <= p_ && p_ < num_patches);

     //bool eplusplus = false;
     bool pplusplus = false;
     for (int i = DIM_d-1; i >= 0; i--) {
       const int last_index = (e_[i] == 0
			       ? ((frame_->bases()[p_])->bases())[i]->DeltaRmax(j_)
			       : ((frame_->bases()[p_])->bases())[i]->Nablamax(j_));
       if (k_[i] == last_index) {
	 k_[i] = (e_[i] == 0
		  ? ((frame_->bases()[p_])->bases())[i]->DeltaLmin()
		  : ((frame_->bases()[p_])->bases())[i]->Nablamin());
	 //eplusplus = (i == 0);
	 pplusplus = (i == 0);
       } else {
	 ++k_[i];
	 break;
       }
     }

     bool eplusplus = false;
     if (pplusplus) {
       eplusplus = !(p_ < num_patches-1);
       p_ = (p_ < num_patches-1) ? p_+1 : 0;
     }
     else return *this;

     bool jplusplus = false;
     if (eplusplus) {
       for (int i = DIM_d-1; i >= 0; i--) {
	 if (e_[i] == 1) {
	   e_[i] = 0;
	   jplusplus = (i == 0);
	 } else {
	   ++e_[i];
	   break;
	 }
       }
       
       if (!jplusplus) // then choose lowest translation index k=k(j,e)
	 for (unsigned int i = 0; i < DIM_d; i++)
	   k_[i] = (e_[i] == 0
		    ? ((frame_->bases()[p_])->bases())[i]->DeltaLmin()
		    : ((frame_->bases()[p_])->bases())[i]->Nablamin());
     }
     
     if (jplusplus) {
       ++j_;
       // choose lowest type e=(0,...,0,1) and lowest translation index k=k(j,e)
       for (unsigned int i = 0; i < DIM_d-1; i++) {
	 e_[i] = 0;
	 k_[i] = ((frame_->bases()[p_])->bases())[i]->DeltaLmin();
       }
       e_[DIM_d-1] = 1;
       k_[DIM_d-1] = ((frame_->bases()[p_])->bases())[DIM_d-1]->Nablamin();
     }
     
     return *this;
   }

   template <class IBASIS, unsigned int DIM_d, unsigned int DIM_m>
   FrameIndex<IBASIS, DIM_d, DIM_m>&
   FrameIndex<IBASIS, DIM_d, DIM_m>::operator = (const FrameIndex& lambda)
   {
     j_ = lambda.j();
     e_ = lambda.e();
     p_ = lambda.p();
     k_ = lambda.k();
     frame_ = lambda.frame();
     
     return *this;
   }


   template <class IBASIS, unsigned int DIM_d, unsigned int DIM_m>
   inline
   bool
   FrameIndex<IBASIS, DIM_d, DIM_m>::operator < (const FrameIndex& lambda) const
   {

     return j_ < lambda.j() ||
      (
       j_ == lambda.j() &&
       (
	e_ < lambda.e() ||
	(
	 e_ == lambda.e() &&
	 (
	  p_ < lambda.p() ||
	  (
	   p_ == lambda.p() && k_ < lambda.k()
	  )
	  )
	 )
	)
      );
   }


   template <class IBASIS, unsigned int DIM_d, unsigned int DIM_m, class FRAME>
   FrameIndex<IBASIS,DIM_d,DIM_m>
   first_generator(const FRAME* frame, const int j)
   {
     assert(j >= frame->j0());
     
     typename FrameIndex<IBASIS,DIM_d,DIM_m>::type_type e;//== 0
     typename FrameIndex<IBASIS,DIM_d,DIM_m>::translation_type k;
     for (unsigned int i = 0; i < DIM_d; i++)
       k[i] = WaveletTL::first_generator<IBASIS>(frame->bases()[0]->bases()[i], j).k();
     
     return FrameIndex<IBASIS,DIM_d,DIM_m>(frame, j, e, 0, k);
   }
   
   template <class IBASIS, unsigned int DIM_d, unsigned int DIM_m, class FRAME>
   FrameIndex<IBASIS,DIM_d,DIM_m>
   last_generator(const FRAME* frame, const int j)
   {
     assert(j >= frame->j0());

     typename FrameIndex<IBASIS,DIM_d,DIM_m>::type_type e;//== 0
     typename FrameIndex<IBASIS,DIM_d,DIM_m>::translation_type k;
     for (unsigned int i = 0; i < DIM_d; i++)
       k[i] = WaveletTL::last_generator<IBASIS>(frame->bases()[0]->bases()[i], j).k();

     return FrameIndex<IBASIS,DIM_d,DIM_m>(frame, j, e, frame->bases().size()-1, k); 
   }

   template <class IBASIS, unsigned int DIM_d, unsigned int DIM_m, class FRAME>
   FrameIndex<IBASIS,DIM_d,DIM_m>
   first_wavelet(const FRAME* frame, const int j)
   {
     assert(j >= frame->j0());

     typename FrameIndex<IBASIS,DIM_d,DIM_m>::type_type e;//== 0
     typename FrameIndex<IBASIS,DIM_d,DIM_m>::translation_type k; 
     for (unsigned int i = 0; i < DIM_d-1; i++)
       k[i] = WaveletTL::first_generator<IBASIS>(frame->bases()[0]->bases()[i], j).k();

     k[DIM_d-1] = WaveletTL::first_wavelet<IBASIS>(frame->bases()[0]->bases()[DIM_d-1], j).k();
     e[DIM_d-1] = 1;

     return FrameIndex<IBASIS,DIM_d,DIM_m>(frame, j, e, 0, k); 
   }

   template <class IBASIS, unsigned int DIM_d, unsigned int DIM_m, class FRAME>
   FrameIndex<IBASIS,DIM_d,DIM_m>
   last_wavelet(const FRAME* frame, const int j)
   {
     assert(j >= frame->j0());
     
     typename FrameIndex<IBASIS,DIM_d,DIM_m>::type_type e;//== 0
     typename FrameIndex<IBASIS,DIM_d,DIM_m>::translation_type k; 
     for (unsigned int i = 0; i < DIM_d; i++) {
       k[i] = WaveletTL::last_wavelet<IBASIS>(frame->bases()[frame->bases().size()-1]->bases()[i], j).k();
       e[i] = 1;
     }
          
     return FrameIndex<IBASIS,DIM_d,DIM_m>(frame, j, e, frame->bases().size()-1, k); 
   }
   
   
   template <class IBASIS, unsigned int DIM_d, unsigned int DIM_m>
   inline
   std::ostream&
   operator << (std::ostream& os, const FrameIndex<IBASIS, DIM_d, DIM_m>& lambda)
   {
     os << "("
	<< lambda.j()
	<< ","
	<< lambda.e()
	<< ","
	<< lambda.p()
	<< ","
	<< lambda.k()
	<< ")";
    return os;

   }



 }
