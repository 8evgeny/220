#include "tmml.hpp"

using namespace std;
using namespace cv;

tmml::tmml(bool &ok, float& min_max_Val)
{
    cout <<"START Constructor tmml\n";
#ifndef NO_GPU
    cuda_Malloc();
#endif // END ifndef NO_GPU
    ok = true;
    cout << "Constructor tmml, ok= " << ok << endl;
} // END tmml

tmml::~tmml()
{
#if defined USE_CUDA
    cuda_Free();
#endif // END #if defined USE_CUDA
    cout << "Destructor tmml" << endl;
} // END ~tmml

#ifdef NO_GPU
   void tmml::work_tmml(const Mat& img_work, const Mat& img_temp, Pix& max_pix)
   {
  #if not defined COMBINED
      matchTemplate(img_work, img_temp, img_result, CORR_TYPE);  // TM_SQDIFF_NORMED, TM_CCOEFF_NORMED.
      minMaxLoc(img_result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
  #endif // END if not defined COMBINED
  #if defined COMBINED
       matchTemplate(img_work, img_temp, img_result2, TM_SQDIFF_NORMED);
       matchTemplate(img_work, img_temp, img_result6, TM_CCOEFF_NORMED);
       minMaxLoc(img_result6 - img_result2, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
  #endif // END defined COMBINED
       max_pix.x = maxLoc.x;
       max_pix.y = maxLoc.y;
       max_pix.bright = maxVal;
   }   // END void work_tmml
#endif // END ifdef NO_GPU
