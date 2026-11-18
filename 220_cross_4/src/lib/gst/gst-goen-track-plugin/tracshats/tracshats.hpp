#ifndef TracShats_H
#define TracShats_H

#include <iostream>
#include <cmath>
#include <fstream>
#include <cmath>
#include <string>
#include <dirent.h>

#include <opencv2/core/utility.hpp>
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/core/core.hpp>

#ifdef GUI_OK
   #include <opencv2/highgui/highgui.hpp>
   #include "opencv2/highgui.hpp"
#endif // END ifdef GUI_OK
#include "trac_api.hpp"

using namespace std;
using namespace cv;
using namespace chrono;

class TracShats
{
 public:
    TracShats(const std::string& config_path, cv::Mat& frame_process_0, bool& ok);
    ~TracShats();

    shared_ptr<class_name> trac = nullptr;
    bool update(cv::Mat &img, cv::Rect2f &target);
    void getSearchRect(Rect2f& rct);
    cv::Point2f getTargetCenter();
    bool init(const Rect2f& rct_in);
    void deinit();
    void setOriginalFrameSize(cv::Mat& frame_process_0);
    bool isInited();
    void setTrackerId(int id);
    void setWorkNumber(int cnt);
    int work();
    bool process();

    int fr_w0 = 1920;
    int fr_h0 = 1080;
    float fr_w0_1, fr_h0_1;
    trac_struct trac_str;
    int find_ = 0, fr_w_show = 0, fr_h_show = 0;

 private:
   string way2img_folder, win_name_tot = "Not zahvat", win_name_tot_old = win_name_tot;
   int num_fr_0, num_fr_1, wait_ms, camera_on, device_id;
  //  int get_rect_manual = 0; // не используется
   int cfg_w, cfg_h, rotate_frame_180, img_show_type, cam_num, show_mode;
   Rect2f object_rect_show;   
   Mat img_orig, tot_show_img;
   Size sz_show;
   float show_orig;
   bool rect_manual_first = 0;
   cv::Point2f wh_local_orig;
   vector<string> img_names;
   const float ext_wh_2 = 239.f / 48.f; // Расширение полурамки локального захвата по умолчанию.
   const float ext_wh = 2.f * ext_wh_2;
   Rect2f rct_local_orig;
   string way2init_rect;

   Scalar red =  Scalar(0, 0, 255);
   Scalar blue = Scalar(255, 0, 0);
   Scalar green = Scalar(0, 255, 0);
   Scalar white = Scalar(255, 255, 255);
   Scalar black = Scalar(0, 0, 0);
   Scalar viol = Scalar(255, 0, 255);
   Scalar cyan = Scalar(255, 255, 0);
   Scalar yell = Scalar(0, 255, 255);
   //***************
   int scaling = 0;
   bool first_frame = true;
   //***************
   std::chrono::duration<double> duration1, duration2, duration3, duration_tot; // Продолжительность времени между кадрами.
   std::chrono::system_clock::time_point time_point0, time_point1, time_point2, time_point3, time_point_old; // Временные точки для нахождения времени обработки кадра.

   bool FileIsExist(const string& filePath);
   bool dirContent(const string& fold, vector<string>& img_names, int typ);
   bool get_ini_params(const string& config);
   inline bool img_orig_prepare();
}; // -- END class TracShats
#endif // TracShats_H

