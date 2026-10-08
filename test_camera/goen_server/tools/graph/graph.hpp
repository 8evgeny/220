#pragma once
#ifndef graph_HPP
#define graph_HPP

#include "tools/colors.h"
#include <cstring>
#include <fstream>
#include <string>
#include <vector>
#include <dirent.h>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <stdio.h>
#include <iostream>
#include "INIReader.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

#ifdef USE_GUI
   #include <opencv2/highgui/highgui.hpp>
   #include "opencv2/highgui.hpp"
#endif // END ifdef USE_GUI


class Graph
{
public:
    Graph(bool & ok);
    void show();
    void init();
    void coord_plane();
    void draw_coord_plane();
    cv::Point ZX_transform(cv::Point vec);
    void draw_vector_coords(float x_res, float y_res, float z_res);
    int draw_rotate_vec(float yaw, float pitch, float roll, float x_res, float y_res, float z_res);
    cv::Point transform_vec(cv::Point src0, cv::Point src1, cv::Point src2, cv::Point dst0, cv::Point dst1, cv::Point dst2, cv::Point vec);

    std::string winname = "graph";
    const float eps = 1e-6;
    double k_deg2rad = 180.0 / M_PI;
    float t_sphere = 0, a_sphere = 0, k_sphere = 0, t_board = 0, a_board = 0, k_board = 0;
    float t_res_ = 0, a_res_ = 0, k_res_ = 0;
    float err = 9999999.f;

    cv::Scalar blue;
    cv::Scalar green;
    cv::Scalar red ;
    cv::Scalar black ;
    cv::Scalar white;

    int width = 800;
    cv::Mat img_rotate_vec;
    int thick_line = 0, thick_text = 0;
    cv::Point pt_zero;  // центр координат
    int axis_len = 0;       // длина осей Z, Y
    int vec_len = 0;        // длина единичного отрезка
    float ax_angle = 0;     // угол оси Х
    float ax_scale = 0;     // масштабирование отрезков на оси X
    cv::Point z_ax;     // оси координад
    cv::Point z_ax_;
    cv::Point y_ax;
    cv::Point y_ax_;
    cv::Point x_ax;
    cv::Point x_ax_;
}; //END class graph

#endif // END graph_HPP
