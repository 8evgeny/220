#pragma once
#define _USE_MATH_DEFINES
#include <opencv2/core/utility.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <cmath>
#include <string>
#include <vector>
#include <iomanip>
#include <mutex>
#include <thread>

const int threads_match_temp = 128;
const int SOURCE_WIDTH = EXT_VAL * TEMPLATE_WIDTH; // 240
const int WORK_WIDTH = SOURCE_WIDTH - 1; // 239
const int RESULT_WIDTH = WORK_WIDTH - TEMPLATE_WIDTH + 1; // 192
const float RESULT_WIDTH_1 = 1.f / RESULT_WIDTH;
const int RESULT_AREA = RESULT_WIDTH * RESULT_WIDTH;
const int WORK_AREA = WORK_WIDTH * WORK_WIDTH;
const int TEMPLATE_AREA = TEMPLATE_WIDTH * TEMPLATE_WIDTH;
const float RESULT_AREA_1 = 1.f / RESULT_AREA;
const float TEMPLATE_WIDTH_1 = 1.f / TEMPLATE_WIDTH;
const float TEMPLATE_AREA_1 = 1.f / TEMPLATE_AREA;
const float KOEFF2LIB_float = KOEFF2LIB;

#ifdef USE_CUDA
    const float threads_match_temp_1 = 1.f / threads_match_temp;
    // Число нитей CUDA в итерации:
    const int blocks_match_temp = RESULT_AREA * threads_match_temp_1;
#endif // END #ifdef USE_CUDA

struct Pix
{
    unsigned char x = 0;
    unsigned char y = 0;
    float bright = 0;
}; // END struct Pix

class tmml
{
  public:
    tmml(bool& ok, float& min_max_Val);
    ~tmml();
    void cuda_Free();
    void work_tmml(const cv::Mat& img_work, const cv::Mat& img_temp, Pix& max_pix);
    const Pix max_pix0;
    Pix max_pix = max_pix0;

  private:
#ifdef NO_GPU
    double maxVal = 0, minVal = 0;
    cv::Point minLoc = cv::Point(0, 0), maxLoc = cv::Point(0, 0);
    cv::Mat img_result = cv::Mat(cv::Size(RESULT_WIDTH, RESULT_WIDTH), CV_32FC1, cv::Scalar(0));
    cv::Mat img_result2 = cv::Mat(cv::Size(RESULT_WIDTH, RESULT_WIDTH), CV_32FC1, cv::Scalar(0));
    cv::Mat img_result6 = cv::Mat(cv::Size(RESULT_WIDTH, RESULT_WIDTH), CV_32FC1, cv::Scalar(0));
#endif // END ifdef NO_GPU
#ifdef USE_CUDA
    void cuda_Malloc();
    Pix * dev_max_pix = nullptr;
    int * dev_val = nullptr;
    const int val0 = 0;
    unsigned char * dev_img_work_arr = nullptr;
    float error_Val = 0.f;
#endif // END #ifdef USE_CUDA
}; // END class tmml
