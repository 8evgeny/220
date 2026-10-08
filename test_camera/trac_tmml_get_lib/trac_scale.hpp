#pragma once
#ifndef trac_tmml_H
#define trac_tmml_H

#include <opencv2/core/utility.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/video/tracking.hpp"

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <cmath>
#include <memory>
#include <stdio.h>
#include <fstream>
#include <chrono>
#include <cmath>
#include <string>
#include <condition_variable>
#include <mutex>
#include <map>
#include <list>
#include <thread>
#include <future>
#include <dirent.h>
#include "INIReader.h"

#if defined OPEN_CL
   #include "tmml_cl.hpp"
#endif // END #if defined OPEN_CL
#if defined USE_CUDA || defined NO_GPU
   #include "tmml.hpp"
#endif // END #if defined USE_CUDA || defined NO_GPU

#include TRACK_API

class trac_tmml
{
  public:
    trac_tmml(const std::string& config_path, bool& ok);
    ~trac_tmml();
    int work();
    void deinit();
    std::unique_ptr<trac_struct> ts = nullptr; // Структура трекинга.

 #if defined OPEN_CL
    std::unique_ptr<tmml_cl> tm = nullptr;
 #endif // END #if defined OPEN_CL
 #if !defined OPEN_CL
    std::unique_ptr<tmml> tm = nullptr;
 #endif // END !defined OPEN_CL

    cv::Mat img_orig, img_orig_roi; // Матрицы для оригинального кадра и для ROI из оригинального кадра.
    cv::Rect2f rct_local_orig; // Рект для ROI на оригинальном кадре.
    int fr_w0 = 0, fr_h0 = 0;

 private:    
    const int blur_size0 = 8; // Ширина размазывания блуром.
    const cv::Size blur_size = cv::Size(blur_size0, blur_size0);
    int work_number_rel = 0;
    const float not_ok_match_count_max = 3.f; // Максимальное время после потери захвата в секундах, после которого захват сбрасывается (deinit()).
    bool first_img = 1;
    std::list<cv::Mat> list_et;
    const float object_relative_max = 0.49; // Максимальное значение безразмерной координаты объекта при захвате.
    bool renew = 0;
    bool shift_ok = 0;
    int num_frame_ok = 0;
    cv::Mat img_et, et, img_local_work;
    int list_et_sz = 8; // Максимальное число сохраняемых эталонов.
    int k_renew_ok = 5; // Число кадров, пропускаемых перед сохранением эталона.
    int k_renew = k_renew_ok;
    int validate_opt = 15;
    int validate_opt_1 = validate_opt - 1;
    int validate_max = 40;
    int validate_deinit = -120; // Значение валидации, ниже которой происходит deinit().

    const int w_et = TEMPLATE_WIDTH; // Ширина стороны квадрата для эталона на рабочем фрейме.
    const int w_2_et = round(0.5 * TEMPLATE_WIDTH); // Полуширина стороны квадрата для эталона на рабочем фрейме.
    const cv::Point wh_et_2 = cv::Point(w_2_et, w_2_et);
    const cv::Point2f wh_et_2f = cv::Point2f(0.5 * TEMPLATE_WIDTH, 0.5 * TEMPLATE_WIDTH);
    const cv::Size TEMPLATE_SIZE = cv::Size(TEMPLATE_WIDTH, TEMPLATE_WIDTH);
    const cv::Size WORK_SIZE = cv::Size(WORK_WIDTH, WORK_WIDTH);
    const float WORK_WIDTH_2 = 0.5 * WORK_WIDTH;
    const float WORK_WIDTH_1 = 1.f / WORK_WIDTH;
    cv::Rect TEMPLATE_RECT = cv::Rect(0, 0, TEMPLATE_WIDTH, TEMPLATE_WIDTH);

    float work2orig_w, work2orig_h, orig2work_w, orig2work_h, min_max_Val, min_max_Val2;
    int fr_h_work, num_fr_0, num_fr_1, fr_w_work;
    cv::Point2f p__  = cv::Point2f(-1000, -1000);
    cv::Point2f center = p__, center_prev = p__, wh_sm_2, center_sm, local_center_tmp, center_kalman;
    cv::Rect2f rct_result, rct_result_orig, rct_result_sm;
    float d02, d02_0;

    cv::Mat result, flow, img_et_sm, result_sm, result_sm1, result_sm2;
    const float koef_f_midl = 0.1; // Переменная для оптического потока (пока не используется)
    const int wh_sm__2_min = 6; // Размер внутреннего шаблона (маленького)
    const float koef_wh_sm = 0.1; // Для вычисления размеров внутреннего шаблона (маленького) из размеров внешнего шаблона.
    const float min_max_Val_sm = 0.76; // Трешхолд для матчинга шаблона по умолчанию.
    int wh_sm__2 = wh_sm__2_min;
    const int win_x = 10; // Отступ окна слева
    const int win_y = 10; // Отступ окна сверху
    cv::Point2f center_orig, wh_local_orig, lt_local_work;

    // -- Для Калмана:
    const float noise_proc = 2e-14; // 1e-4; // Шум процесса.
    const float noise_measurement = 4; // 0.1; // Шум измерения.
    const float err_renew = 0.1; // Обновление ошибки ковариации.
    const float dt_tr = 1e-5; // 1.f; // Время запаздывания для сглаживания траекторий (пропорционально качеству сглаживания Калмана).
    const float dt_tr_1 = 1.f/dt_tr;
    cv::KalmanFilter KF_tr = cv::KalmanFilter(4, 2, 0);
    cv::Mat_<float> measurement_tr = cv::Mat_<float>(2, 1);
    cv::Mat est, pred;

    double minVal, minVal_sm, maxVal_sm;
    cv::Point minLoc, maxLoc, minLoc_sm, maxLoc_sm;
    cv::Point2f matchLoc, matchLoc_sm;
#if defined(USE_smooth)
    struct smoth_trac{float x = 0.5; float y = 0.5; size_t work_number = 0;};
    std::list<smoth_trac> list_st;
    int list_sz_min = 10; // Минимальное число точек на траектории (не меньше 2).
    int Polinom_size = 1; // Степень полинома интерполяции.
    cv::Mat A, A_inv;
#endif // END #if defined(USE_smooth)
    float shift2 = 0.004; // Максимальный относительный квадрат смещения текущей рамки от предыдущей

    std::chrono::duration<double> duration1, duration_delay, not_ok_match_duration; // Продолжительность времени между кадрами.
    std::chrono::system_clock::time_point time_point1_old, time_point0, time_point1, not_ok_match_start, not_ok_match_stop; // Временные точки для нахождения времени обработки кадра.
    float duration_delay_fps = 0, max_fps = 0;    

#if defined(USE_scale)
   bool first_match = 1; // Признак первого кадра захвата для LK.
   // Для LK-алгоритма:
   const int N_LK_2 = 32; // Число точек на окружности.
   const int N_LK = 2 * N_LK_2; // Полное число точек на обеих окружностях.
   const float rad1_rel = 0.5; // Относительный радиус первой окружности в единицах TEMPLATE_WIDTH.
   const float rad2_rel = 1.5; // Относительный радиус второй окружности в единицах TEMPLATE_WIDTH.
   const double rad1 = rad1_rel * TEMPLATE_WIDTH;
   const double rad2 = rad2_rel * TEMPLATE_WIDTH;
   const cv::Size wsz = cv::Size(7, 7); // Окно сглаживания оптического потока (обратно-пропорционально скорости работы).
   const int list_scale_size = 20;
   const float list_scale_size_2 = 0.5 / list_scale_size;
   const float abs_scale0_max = 0.1;
   float sum_list_scale = 0;
   std::list<float> list_scale;
   std::vector<cv::Point2f> v_angl, v_LK_points; // Вектор косинусов-синусов и вектор точек для LK.
   cv::Mat img4LK_prev; // Матрицы для LK и для подготовки матриц для LK.
   cv::TermCriteria criteria = cv::TermCriteria((cv::TermCriteria::COUNT) + (cv::TermCriteria::EPS), 10, 0.03);

   void get_vec_angl();
   void get_vec_LK_points();
   float get_scale();
#endif // END #if defined(USE_scale)

    bool get_img_local_work();
    bool match_img();
    bool get_cmd_result(const string& get_disk_id, const std::vector<string>& v_disc_id);
    bool FileIsExist(const string& filePath);
    bool shift_verify(const cv::Point2f& p);
    bool verify_pnt(const cv::Rect2f& rct, const cv::Point2f& p);
    bool verify_rect(const cv::Size& sz, const cv::Rect2f& r);
    void init_work();
    bool get_ini_params(const string& config);
#if defined(USE_smooth)
    bool get_obj_xy_smooth(float obj_xy_x, float obj_xy_y);
    bool get_abc(const std::vector<cv::Point2d>& vec, std::vector<float>& B);
    bool get_smooth_xy(cv::Point2f& extr_xy);
#endif // END #if defined(USE_smooth)
}; // END class trac_tmml
#endif // END #ifndef trac_tmml_H
