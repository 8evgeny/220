#include "trac_scale.hpp"

using namespace std;
using namespace cv;
using namespace chrono;

bool trac_tmml::get_cmd_result(const string& get_disk_id, const vector<string>& v_disc_id)
{
    array<char, 128> buffer;
    string cmd_result = "";
    unique_ptr<FILE, decltype(&pclose) > pipe(popen(get_disk_id.c_str(), "r"), pclose);
    if(!pipe){throw std::runtime_error("popen() failed!");}
    while(fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr){cmd_result += buffer.data();}
    int num_pos = cmd_result.find("=") + 1;
    int len = cmd_result.length() - num_pos - 1;
    string disc_id = cmd_result.substr(num_pos, len);
    bool ok = 0;
    for(int i=0; i < v_disc_id.size(); i++)
    {
        if(v_disc_id[i] == disc_id){ok = 1; break;}
    } // -- END for(int i=0; i < v_disc_id.size(); i++)
    return ok;
} // -- END get_cmd_result

trac_tmml::trac_tmml(const std::string& config_path, bool& ok)
{
   ok = get_ini_params(config_path);
   if(!ok){return;}
#if defined OPEN_CL
   tm = make_unique<tmml_cl>(ok, min_max_Val);
   tm->max_pix = tm->max_pix0;
#endif // END #if defined OPEN_CL
#if !defined OPEN_CL
   tm = make_unique<tmml>(ok, min_max_Val);
   tm->max_pix = tm->max_pix0;
#endif // END !defined OPEN_CL
 // -------------------------- FIND DISC_ID -----------------------------------------------
 #ifdef FIND_DISC_ID
   vector<string> v_disc_id = {
       "114A4G5FS",
       "BTHH83700P5L512D",
       "ZN1BFQHB",
       "Z524F25F",
       "2104A9474309",
       "2J4120178746",
       "WD-WCC6Y6JDRY7P",
       "S4BENG0KC32655V",
       "2L1529A64EGX",
       "2L1529ABGJNP",
       "S3PRD9NP",
       "S649NL0TB45870W"
   };
   string cmd_result1 = "udevadm info --query=all --name=/dev/sda | grep ID_SERIAL_SHORT";
   string cmd_result2 = "udevadm info --query=all --name=nvme0n1 | grep ID_SERIAL_SHORT";
   string cmd_result3 = "udevadm info --query=all --name=/dev/sdb | grep ID_SERIAL_SHORT";
   string cmd_result4 = "udevadm info --query=all --name=/dev/sdc | grep ID_SERIAL_SHORT";
   ok = get_cmd_result(cmd_result1, v_disc_id);
   if(!ok){ok = get_cmd_result(cmd_result2, v_disc_id);}
   if(!ok){ok = get_cmd_result(cmd_result3, v_disc_id);}
   if(!ok){ok = get_cmd_result(cmd_result4, v_disc_id);}
   if(!ok){cout << "=============== Wrong HARDWARE !" << endl; return;}
 #endif // -------------------------- END FIND DISC_ID -----------------------------------------------
   KF_tr.transitionMatrix = (Mat_<float>(4, 4) << 1, 0, dt_tr_1, 0,      0, 1, 0, dt_tr_1,       0, 0, 1, 0,       0, 0, 0, 1);
   measurement_tr.setTo(Scalar(1));
   setIdentity(KF_tr.measurementMatrix); // Инициализация матрицы измерений
   setIdentity(KF_tr.processNoiseCov, Scalar::all(noise_proc)); // Значение ковариации шума процесса
   setIdentity(KF_tr.measurementNoiseCov, Scalar::all(noise_measurement)); // Значение ковариации шума измерения
   setIdentity(KF_tr.errorCovPost, Scalar::all(err_renew)); // обновление ошибки ковариации 
#if defined(USE_smooth)
   A = Mat(Size(Polinom_size, Polinom_size), CV_64F);
   A_inv = Mat(Size(Polinom_size, Polinom_size), CV_64F);
#endif // END #if defined(USE_smooth)

#if defined(USE_scale)
   get_vec_angl();
   get_vec_LK_points();   
#endif // END #if defined(USE_scale)

   time_point0 = system_clock::now();
   time_point1 = system_clock::now();
} // -- END trac_tmml

trac_tmml::~trac_tmml()
{    
  #if defined USE_CUDA
    tm->cuda_Free();
  #endif // END #if defined USE_CUDA
  #if defined OPEN_CL
    tm->CL_FREE();
  #endif // END #if defined OPEN_CL
    tm.release();
    ts.release();
    cout << "Destructor trac_tmml" << endl;
} // END ~trac_tmml()

shared_ptr<trac_tmml> create_track(const char * config_path, bool& ok, trac_struct& trac_st)
{
    shared_ptr<trac_tmml> trac = make_shared<trac_tmml>(config_path, ok);
    trac->ts = unique_ptr<trac_struct>(&trac_st);
    return trac;
} // -- END create_trac

int get_trac(shared_ptr<trac_tmml>& trac){return trac->work();}

void trac_tmml::deinit()
{
    ts->zahvat = 0;
    list_et.clear();
    first_img = 1;
    k_renew = k_renew_ok;
    ts->ok_match = 0;
    ts->validate = validate_opt;
    ts->rect_ok = 0;
    img_orig.data = nullptr;
    measurement_tr.setTo(Scalar(0));
#if defined(USE_smooth)
    list_st.resize(0);
    d02 = d02_0;
#endif // END #if defined(USE_smooth)
#if defined(USE_scale)
    first_match = 1;
    sum_list_scale = 0;
    list_scale.resize(0);    
#endif // END #if defined(USE_scale)
} // -- END deinit

bool trac_tmml::get_ini_params(const string& config)
{
   cout << endl << "BEGIN get_ini_params trac" << endl;
   if(!FileIsExist(config)){cout << "File '" << config << "' not exist!" << endl; return 0;}
   INIReader reader(config);
   if(reader.ParseError()<0){cout << "Can't load '" << config << "'\n"; return 0;}
   setlocale(LC_NUMERIC, "en_US.UTF-8");
   // --------------------------------------------------- tracking:

   k_renew_ok = reader.GetInteger("tracking", "k_renew_ok", -1);
   if(k_renew_ok == -1){cout << "k_renew_ok not declared!\n"; return 0;}
   else{k_renew = k_renew_ok; cout << "k_renew_ok = " << k_renew_ok << ";\n";}

   list_et_sz = reader.GetInteger("tracking", "list_et_sz", -1);
   if(list_et_sz == -1){cout << "list_et_sz not declared!\n"; return 0;}
   else{cout << "list_et_sz = " << list_et_sz << ";\n";}

   min_max_Val = reader.GetReal("tracking", "min_max_Val", -1);
   if(min_max_Val == -1){cout << "min_max_Val not declared!\n"; return 0;}
   else{cout << "min_max_Val = " << min_max_Val << ";\n";}
   min_max_Val2 = min_max_Val * min_max_Val;

   shift2 = reader.GetReal("tracking", "shift2", -1);
   if(shift2 == -1){cout << "shift2 not declared!\n"; return 0;}
   else{cout << "shift2 = " << shift2 << ";\n";}
   d02_0 = shift2 * TEMPLATE_SIZE.area();
   d02 = d02_0;

#if defined(USE_smooth)
   Polinom_size = reader.GetInteger("tracking", "Polinom_size", -1);
   if(Polinom_size == -1){cout << "Polinom_size not declared!\n"; return 0;}
   else{cout << "Polinom_size = " << Polinom_size << ";\n";}

   list_sz_min = reader.GetInteger("tracking", "deq_sz_min", -1);
   if(list_sz_min == -1){cout << "deq_sz_min not declared!\n"; return 0;}
   else{cout << "list_sz_min = " << list_sz_min << ";\n";}
#endif // END #if defined(USE_smooth)

   validate_deinit = reader.GetInteger("tracking", "validate_ext", -1);
   if(validate_deinit == -1){cout << "validate_ext not declared!\n"; return 0;}
   else{cout << "validate_deinit = " << validate_deinit << ";\n";}

   max_fps = reader.GetInteger("tracking", "max_fps", -1);
   if(max_fps == -1){cout << "max_fps not declared!\n"; return 0;}
   else{cout << "max_fps = " << max_fps << ";\n";}
   duration_delay_fps = 1.f/max_fps;
   cout << "END get_ini_params trac" << endl << endl;
   return 1;
} // -- END get_ini_params

bool trac_tmml::FileIsExist(const string& filePath)
{
    bool isExist = false;
    ifstream fin(filePath.c_str());
    if(fin.is_open()){isExist = true;}
    fin.close();
    return isExist;
} // -- END FileIsExist

#if defined(USE_scale)
  void trac_tmml::get_vec_angl()
  {
     v_angl.resize(N_LK_2);
     float dfi = 2.f * M_PI / N_LK_2;
     float fi0 = 0.5 * dfi;
     for(int i = 0; i < N_LK_2; i++)
     {
         float fi = fi0 + i * dfi;
         v_angl[i] = Point2f(cos(fi), sin(fi));
     } // -- END for(int i = 0; i < N_LK_2; i++)
  } // -- END get_vec_angl

  void trac_tmml::get_vec_LK_points()
  {
      v_LK_points.resize(N_LK);
      for(int i = 0; i < N_LK_2; i++)
      {
          v_LK_points[i] = Point2f(WORK_WIDTH_2 + rad1 * v_angl[i].x, WORK_WIDTH_2 + rad1 * v_angl[i].y);
          v_LK_points[N_LK_2 + i] = Point2f(WORK_WIDTH_2 + rad2 * v_angl[i].x, WORK_WIDTH_2 + rad2 * v_angl[i].y);
      } // -- END for(int i = 0; i < N_LK_2; i++)
  } // -- END get_vec_LK_points

  float trac_tmml::get_scale()
  {
     if(first_match)
     {
         first_match = 0;
         img4LK_prev = img_local_work.clone();
         return 1.f;
     } // -- END if(first_match)

     // calculate optical flow
     vector<Point2f> predict;
     vector<unsigned char> status;
     vector<float> err;
     calcOpticalFlowPyrLK(img4LK_prev, img_local_work, v_LK_points, predict, status, err, wsz, 2, criteria);
     double sum_flow1 = 0, sum_flow2 = 0, sum_rad1 = 0, sum_rad2 = 0;
     for(int i1 = 0; i1 < N_LK_2; i1++)
     {
         Point2f& v_angl_i = v_angl[i1];
         if(status[i1] == 1)
         {
            Point2f flow1 = predict[i1] - v_LK_points[i1];
            sum_flow1 += (flow1.x * v_angl_i.x + flow1.y * v_angl_i.y);
            sum_rad1 += rad1;
         } // END if(status[i1] == 1)
         int i2 = N_LK_2 + i1;
         if(status[i2] == 1)
         {
            Point2f flow2 = predict[i2] - v_LK_points[i2];
            sum_flow2 += (flow2.x * v_angl_i.x + flow2.y * v_angl_i.y);
            sum_rad2 += rad2;
         } // END if(status[i2] == 1)
     } // -- END for(int i1 = 0; i1 < N_LK_2; i1++)
     img4LK_prev = img_local_work.clone();
     float scale0 = (float)(sum_flow1 / sum_rad1 + sum_flow2 / sum_rad2);
     // cout << "scale0=" << scale0 << "\n"; // << "; k_ext1=" << k_ext1 << "; k_ext2=" << k_ext2 << endl;
     if(abs(scale0) < abs_scale0_max)
     {
         list_scale.push_front(scale0);
         sum_list_scale += scale0;
     } // END if(abs(scale0) < abs_scale0_max)
     else
     {
         list_scale.push_front(0);
     } // END if(abs(scale0) >= abs_scale0_max)
     if(list_scale.size() > list_scale_size)
     {
         sum_list_scale -= list_scale.back();
         list_scale.pop_back();
         return 1.f + sum_list_scale * list_scale_size_2;
     } // END if(list_scale.size() > list_scale_size)

     return 1.f;
  } // -- END get_scale
#endif // END #if defined(USE_scale)

bool trac_tmml::verify_pnt(const Rect2f& r, const Point2f& p)
{
    if(p.x < r.tl().x || p.y < r.tl().y || p.x >= r.br().x || p.y >= r.br().y){return 0;}
    return 1;
} // -- END verify_pnt

bool trac_tmml::verify_rect(const Size& sz, const Rect2f& r)
{
    if(r.x < 0 || r.y < 0 || r.br().x >= sz.width || r.br().y >= sz.height){return 0;}
    return 1;
} // -- END verify_rect

bool trac_tmml::shift_verify(const Point2f& pnt)
{    
    Point2f dp = pnt - center_prev;
    float d2 = dp.x * dp.x + dp.y * dp.y;
    work_number_rel = ts->work_number - num_frame_ok;
    if(d2 < d02 * work_number_rel * work_number_rel){return 1;}
    return 0;
} // -- END shift_verify

bool trac_tmml::get_img_local_work()
{
    Point2f wh_local_orig(ts->roi_w, ts->roi_h);
    center_orig = Point2f(ts->obj_xy_x * fr_w0, ts->obj_xy_y * fr_h0);
    Point2f lt = center_orig - 0.5 * wh_local_orig;
    if(lt.x < 0){lt.x = 0;}
    if(lt.y < 0){lt.y = 0;}
    Point2f br = lt + wh_local_orig;
    if(br.x >= fr_w0){br.x = fr_w0 - 1;}
    if(br.y >= fr_h0){br.y = fr_h0 - 1;}
    Point2f br_wh1 = br - wh_local_orig;
    if(br_wh1.x < lt.x){lt.x = br_wh1.x;}
    if(br_wh1.y < lt.y){lt.y = br_wh1.y;}
    if(lt.x < 0 || lt.y < 0){return 0;}
    rct_local_orig = Rect2f(lt, br);
    lt_local_work = Point2f(orig2work_w * rct_local_orig.x, orig2work_h * rct_local_orig.y);
    img_orig_roi = img_orig(rct_local_orig);
    resize(img_orig_roi, img_local_work, WORK_SIZE);
    //equalizeHist(img_local_work, img_local_work);
    //normalize(img_local_work, img_local_work, 0, 255, NORM_MINMAX);
    blur(img_local_work, img_local_work, blur_size);
    return 1;
} // END get_img_local_work

bool trac_tmml::match_img()
{
   if(!get_img_local_work()){return 0;}
   int k = 0;
   for(auto it = list_et.begin(); it != list_et.end();)
   {
      // ------------------------------ Нахождение et на изображении img_work_local
      et = it->clone();
      it++;
      tm->work_tmml(img_local_work, et, tm->max_pix);
      if(tm->max_pix.bright > min_max_Val)
      {
          TEMPLATE_RECT.x = tm->max_pix.x;
          TEMPLATE_RECT.y = tm->max_pix.y;
          if(!verify_rect(WORK_SIZE, TEMPLATE_RECT)){k++; continue;}
          local_center_tmp = TEMPLATE_RECT.tl() + wh_et_2;
#ifdef USE_img_et_sm
          Mat img_et_tmp = img_local_work(TEMPLATE_RECT);
          // ------------------------------ Нахождение img_et_sm на изображении img_et
          img_et_sm = et(rct_result_sm);
          /// Do the Matching and Normalize
      #ifdef COMBINED
          matchTemplate(img_et_tmp, img_et_sm, result_sm1, TM_CCOEFF_NORMED);
          matchTemplate(img_et_tmp, img_et_sm, result_sm2, TM_SQDIFF_NORMED);
          minMaxLoc(result_sm1 - result_sm2, &minVal_sm, &maxVal_sm, &minLoc_sm, &maxLoc_sm, Mat());
      #endif // END #ifdef COMBINED
      #ifndef COMBINED
          matchTemplate(img_et_tmp, img_et_sm, result_sm, TM_CCOEFF_NORMED);
          minMaxLoc(result_sm, &minVal_sm, &maxVal_sm, &minLoc_sm, &maxLoc_sm, Mat());
      #endif // END #ifndef COMBINED
          //cout << "maxVal_sm=" << maxVal_sm << endl;
          if(maxVal_sm < min_max_Val){k++; continue;}
          local_center_tmp = Point2f(TEMPLATE_RECT.x + maxLoc_sm.x + wh_sm__2, TEMPLATE_RECT.y + maxLoc_sm.y + wh_sm__2);
#endif // END #ifdef USE_img_et_sm
          center_prev = center;
          center = lt_local_work + local_center_tmp;
          measurement_tr(0) = center.x; // измеренное значение
          measurement_tr(1) = center.y; // измеренное значение          
          if(first_img){shift_ok = 1;}
          else
          {
             shift_ok = shift_verify(center);
             if(!shift_ok){k++; continue;}
          } // -- END if(!first_img)
          rct_result = Rect2f(center - wh_et_2f, center + wh_et_2f);
          rct_result_orig = Rect2f(work2orig_w * rct_result.x, work2orig_h * rct_result.y,
                                   work2orig_w * rct_result.width, work2orig_h * rct_result.height);
          est = KF_tr.correct(measurement_tr); // Обновление
          pred = KF_tr.predict();
          //center_kalman = Point2f(pred.at<float>(0), pred.at<float>(1)); // -- Калман.
          center_kalman = Point2f(est.at<float>(0), est.at<float>(1)); // -- Калман.
  #if defined(USE_scale)
          float scale = get_scale();
          float w_2 = ts->obj_wh_2_w * scale;
          float h_2 = ts->obj_wh_2_h * scale;
          if(w_2 < 0.090 && h_2 < 0.090 && w_2 > 0.003 && h_2 > 0.003)
          {
              ts->obj_wh_2_w = w_2;
              ts->obj_wh_2_h = h_2;
          } // END if(w_2 < 0.090 && h_2 < 0.090 && w_2 > 0.003 && h_2 > 0.003)
  #endif // END #if defined(USE_scale)
          num_frame_ok = ts->work_number;
          return 1;
      } // -- END if(tm->max_pix.bright > min_max_Val)
      k++;
   } // -- END for(auto it = list_et.begin(); it != list_et.end(); it++)
   measurement_tr(0) = center_kalman.x; // измеренное значение
   measurement_tr(1) = center_kalman.y; // измеренное значение
   est = KF_tr.correct(measurement_tr); // Обновление
   pred = KF_tr.predict();
   center_prev = center;
   //center_kalman = Point2f(pred.at<float>(0), pred.at<float>(1)); // -- Калман.
   center_kalman = Point2f(est.at<float>(0), est.at<float>(1)); // -- Калман.
   center = center_kalman;
   rct_result = Rect2f(center - wh_et_2f, center + wh_et_2f);
   rct_result_orig = Rect2f(work2orig_w * rct_result.x, work2orig_h * rct_result.y,
                            work2orig_w * rct_result.width, work2orig_h * rct_result.height);
   if(ts->validate < validate_deinit){deinit();}
   return 0;
} // END match_img

void trac_tmml::init_work()
{
    k_renew++;
    if(k_renew > k_renew_ok)
    {
        k_renew = 0;
        renew = 1;
        if(!first_img && verify_rect(WORK_SIZE, TEMPLATE_RECT)){img_et = img_local_work(TEMPLATE_RECT).clone();}
        list_et.push_back(img_et);
        if(list_et.size() > list_et_sz){list_et.pop_front();}
    } // -- END if(k_renew > k_renew_ok)
} // -- END init_work

#if defined(USE_smooth) // END #if defined(USE_smooth)
   bool trac_tmml::get_abc(const vector<Point2d>& vec, vector<float>& B)
   {
       auto p = [](double x, int pw)
       {
           double res = 1.f;
           for(int i=0; i < pw; ++i){res *= x;}
           return res;
       }; // END auto p = [](double x, int pw)
       double C[Polinom_size];
       for(int y = 0; y < Polinom_size; ++y)
       {
          for(int x = 0; x < Polinom_size; ++x)
          {
              double SA = 0; int pw = x + y;
              for(int i = 0; i < vec.size(); ++i){SA += p(vec[i].x, pw);}
              A.at<double>(y, x) = SA;
          } // -- END for(int x = 0; x<Polinom_size; x++)
          double SC = 0;
          for(int i = 0; i < vec.size(); ++i){SC += vec[i].y * p(vec[i].x, y);}
          C[y] = SC;
       } // -- END for(int y = 0; y < Polinom_size; y++)
       invert(A, A_inv);
       for(int y = 0; y < Polinom_size; y++)
       {
          double SB = 0;
          for(int x = 0; x < Polinom_size; x++){SB += C[x] * A_inv.at<double>(y, x);}
          B[y] = SB;
       } // -- END for(int y = 0; y<K; y++)
       return 1;
   } // -- END get_abc

   bool trac_tmml::get_smooth_xy(Point2f& extr_xy)
   {
       vector<Point2d> x_out(list_st.size()), y_out(list_st.size());
       size_t ts_0 = list_st.begin()->work_number;
       size_t ts_01 = list_st.back().work_number - ts_0;
       int i_it = 0;
       for(list<smoth_trac>::iterator it = list_st.begin(); it != list_st.end(); it++)
       {
           float ts_i = (float)(it->work_number - ts_0) / (float)ts_01;
           x_out[i_it] = Point2d(ts_i, it->x);
           y_out[i_it] = Point2d(ts_i, it->y);
           i_it++;
       } // END for(list<smoth_trac>::iterator it = list_st.begin(); it != list_st.end(); it++)
       vector<float> B_x(Polinom_size), B_y(Polinom_size);
       bool ok_x = get_abc(x_out, B_x);
       if(!ok_x){return 0;}
       bool ok_y = get_abc(y_out, B_y);
       if(!ok_y){return 0;}
       for(int j = 0; j < Polinom_size; ++j){extr_xy += Point2f(B_x[j], B_y[j]);}
       return 1;
   } // -- END get_smooth_xy

   bool trac_tmml::get_obj_xy_smooth(float obj_xy_x, float obj_xy_y)
   {
       list_st.push_back(smoth_trac{obj_xy_x, obj_xy_y, ts->work_number});
       if(list_st.size() < list_sz_min)
       {
           ts->obj_xy_x = obj_xy_x;
           ts->obj_xy_y = obj_xy_y;
           return 0;
       } // END if(list_st.size() < list_sz_min)
       Point2f extr_xy(0, 0);
       bool ok = get_smooth_xy(extr_xy);
       if(ok)
       {
           ts->obj_xy_x = extr_xy.x;
           ts->obj_xy_y = extr_xy.y;
       } // END if(ok)
       else
       {
           ts->obj_xy_x = obj_xy_x;
           ts->obj_xy_y = obj_xy_y;
           return 0;
       } // -- END if(!ok)
       list_st.pop_front();
       return 1;
   } // -- END get_obj_xy_smooth
#endif // END #if defined(USE_smooth)

int trac_tmml::work()
{
    if(ts->key == 27){deinit(); return 0;}
    if(fr_w0 != ts->fr_w0 || fr_h0 != ts->fr_h0)
    {
        first_img = 1;
        fr_w0 = ts->fr_w0;
        fr_h0 = ts->fr_h0;
    } // END if(fr_w0 != ts->fr_w0 || fr_h0 != ts->fr_h0)
    img_orig = Mat(fr_h0, fr_w0, ts->img_orig_type, ts->img_orig_data);
    if(img_orig.channels() == 3){cvtColor(img_orig, img_orig, COLOR_BGR2GRAY);}
    if(ts->rect_ok == 2){ts->zahvat = 1;}
#ifdef COUT_OK
    time_point0 = system_clock::now();
#endif // END COUT_OK
    if(ts->zahvat)
    {
       if(first_img)
       {
           num_frame_ok = ts->work_number;
           work2orig_w = WORK_WIDTH_1 * ts->roi_w;
           work2orig_h = WORK_WIDTH_1 * ts->roi_h;
           orig2work_w = 1.f / work2orig_w;
           orig2work_h = 1.f / work2orig_h;
           fr_w_work = round(orig2work_w * fr_w0);
           fr_h_work = round(orig2work_h * fr_h0);
           center = Point2f(ts->obj_xy_x * fr_w_work, ts->obj_xy_y * fr_h_work);
           center_prev = center;
           rct_result_orig = Rect2f((ts->obj_xy_x - ts->obj_wh_2_w) * fr_w0,
                                    (ts->obj_xy_y - ts->obj_wh_2_h) * fr_h0,
                                    2.f * ts->obj_wh_2_w * fr_w0,
                                    2.f * ts->obj_wh_2_h * fr_h0);
           if(ts->rect_ok == 2)
           {
               ts->rect_ok = 0;
               if(get_img_local_work())
               {
                  TEMPLATE_RECT.x = round(orig2work_w * (rct_result_orig.x - rct_local_orig.x));
                  if(TEMPLATE_RECT.x < 0){TEMPLATE_RECT.x = 0;}
                  TEMPLATE_RECT.y = round(orig2work_h * (rct_result_orig.y - rct_local_orig.y));
                  if(TEMPLATE_RECT.y < 0){TEMPLATE_RECT.y = 0;}
                  if(TEMPLATE_RECT.br().x < WORK_WIDTH && TEMPLATE_RECT.br().y < WORK_WIDTH)
                  {
                     img_et = img_local_work(TEMPLATE_RECT).clone();
                  } // END if(TEMPLATE_RECT.br().x < WORK_WIDTH && TEMPLATE_RECT.br().y < WORK_WIDTH)
               } // END if(get_img_local_work())
           } // -- END if(rect_ok == 2)
#ifdef USE_img_et_sm
           wh_sm__2 = MAX(wh_sm__2_min, round(koef_wh_sm * wh_et_2.x));
           wh_sm_2 = Point2f(wh_sm__2, wh_sm__2);
           rct_result_sm = Rect2f(wh_et_2f - wh_sm_2, wh_et_2f + wh_sm_2);
           if(!verify_rect(TEMPLATE_SIZE, rct_result_sm)){deinit(); return 0;}
#endif // END #ifdef USE_img_et_sm
           init_work();
           ts->validate = validate_opt;
           KF_tr.statePre.at<float>(0) = center.x; /////////////////
           KF_tr.statePre.at<float>(1) = center.y; // Инициализация первых
           KF_tr.statePre.at<float>(2) = 0; // предсказанных значений
           KF_tr.statePre.at<float>(3) = 0; /////////////////
           measurement_tr(0) = center.x;
           measurement_tr(1) = center.y;           
       } // END if(first_img)
#if defined(USE_scale)
       else
       { // if(!first_img)
           work2orig_w = WORK_WIDTH_1 * ts->roi_w;
           work2orig_h = WORK_WIDTH_1 * ts->roi_h;
           orig2work_w = 1.f / work2orig_w;
           orig2work_h = 1.f / work2orig_h;
           fr_w_work = round(orig2work_w * fr_w0);
           fr_h_work = round(orig2work_h * fr_h0);
           center = Point2f(ts->obj_xy_x * fr_w_work, ts->obj_xy_y * fr_h_work);
       } // END if(!first_img)
#endif // END #if defined(USE_scale)
       ts->ok_match = match_img();       
       if(first_img)
       {
           if(!ts->ok_match){deinit(); return 0;}
           first_img = 0;
       } // -- END if(first_img)
       float obj_xy_x = center.x / fr_w_work;
       float obj_xy_y = center.y / fr_h_work;
#if defined(USE_smooth)
       get_obj_xy_smooth(obj_xy_x, obj_xy_y);
#endif // END #if defined(USE_smooth)
#if !defined(USE_smooth)
           ts->obj_xy_x = obj_xy_x;
           ts->obj_xy_y = obj_xy_y;
#endif // END #if !defined(USE_smooth)
       if(ts->ok_match)
       {
           if(shift_ok)
           {
               ts->validate++;
               if(ts->validate > validate_max){ts->validate = validate_max;}
           } // END if(shift_ok)
           else
           {
               ts->validate--;
               k_renew = k_renew_ok - 1;
           } // -- END if(!shift_ok)

           if(ts->validate > validate_opt_1){init_work();}

           not_ok_match_start = system_clock::now();
       } // END if(ts->ok_match)
       else
       {
           ts->validate--;
           k_renew = k_renew_ok - 1;
           not_ok_match_stop = system_clock::now();
           not_ok_match_duration = not_ok_match_stop - not_ok_match_start;
           if(not_ok_match_duration.count() > not_ok_match_count_max){deinit(); return 0;}
       } // -- END if(!ts->ok_match)
    } // -- END if(ts->zahvat)
#ifdef MAX_FPS_OK
    time_point1_old = time_point1;
    time_point1 = system_clock::now();
#endif // END MAX_FPS_OK
#ifdef COUT_OK
    time_point1 = system_clock::now();    
#endif // END COUT_OK
#ifdef MAX_FPS_OK
    duration_delay = time_point1 - time_point1_old;
    float duration_delay_diff = duration_delay_fps - duration_delay.count();
    if(duration_delay_diff > 0){usleep(1000000 * duration_delay_diff);}
#endif // END MAX_FPS_OK
#ifdef COUT_OK
    duration1 = time_point1 - time_point0;
    cout << ts->id << "->" << ts->work_number << "; shift_ok=" << shift_ok << "; dt1=" << duration1.count() << "; validate=" << ts->validate << "; ok_match=" << ts->ok_match << endl;
#endif // END COUT_OK
    if(ts->ok_match && (abs(ts->obj_xy_x - 0.5) > object_relative_max || abs(ts->obj_xy_y - 0.5) > object_relative_max))
    {
        deinit();
    } // -- END if(ts->ok_match && (abs(ts->obj_xy_x - 0.5) > object_relative_max || abs(ts->obj_xy_y - 0.5) > object_relative_max))
    return 0;
} // -- END work
