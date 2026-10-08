#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <vector>
#include <memory>

#include <INIReader.h>

class Plot
{
public:
    Plot(const std::string & section, const std::string & config_path, bool & ok);
    void set_vv(std::vector<std::vector<cv::Point2f>> & v_xy);

    char show();

    std::string winname = "win";
    cv::Size frame_size = {1080,400};
    std::vector<cv::Scalar> v_clr = {{0,0,255}, {0,255,0}, {100,100,200}, {100, 200, 100}, {255,0,255}, {255,255,0}};
    std::vector<int> v_thik = {1, 1, 2, 2, 1, 2};
    cv::Scalar clr_fone = {255,255,255};
    cv::Scalar clr_line_m = {200,200,200};
    cv::Scalar clr_line = {220,220,220};

    cv::Mat img;

    float X_min = 0, X_max = 1, Y_min = 0, Y_max = 1, X_step = 1, Y_step = 5;
    float x_min = 0, x_max = 1, y_min = 0, y_max = 1, x_step = 1, y_step = 5;
    float x_norm = x_max - x_min;
    float y_norm = y_max - y_min;
    int num_f = 1;
    int plot_w = 1920 ;
    int plot_h = 1080 ;
    std::vector<std::vector<cv::Point2f>> vv_func;
    bool get_ini_params(const std::string &config, const std::string & section); // -- END get_ini_params
}; // END class Plot

