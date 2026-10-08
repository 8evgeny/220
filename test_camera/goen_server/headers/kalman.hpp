#pragma once
#ifndef Kalman_H
#define Kalman_H

#include "INIReader.h"
#include <cstring>
#include <thread>
#include <functional>
#include <signal.h>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <condition_variable>
#include <dirent.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <cmath>
#include <atomic>
#include <deque>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>

class Kalman
{
  public:
    cv::KalmanFilter KF;
    cv::Mat processNoise, measurement, state, prediction;

  private:
    float roll_angle = 0.f;
    float pitch_angle = 0.f;
    std::string config_path = "";
    float dispersion_process = - 1000000000.f;
    float dispersion_measurement = - 1000000000.f;
    float delta_t = -1.f;

  public:
    Kalman(const std::string & pathToConfig, const std::string & block_name, bool &ok) : config_path(pathToConfig)
    {
        std::cout << "config_path = " << config_path << std::endl;
        ok = get_ini_params(config_path, block_name);
        if(!ok){std::cout << "Not ok get_ini_params!" << std::endl; return;}
        float delta_t_1 = 1.f / delta_t;

        KF = cv::KalmanFilter(2, 1, 0);
        processNoise = cv::Mat(2, 1, CV_32F);
        measurement = cv::Mat::zeros(1, 1, CV_32F);
        KF.transitionMatrix = (cv::Mat_<float>(2, 2) << 1, delta_t_1, 0, 1);
        state = cv::Mat(2, 1, CV_32F);
        setIdentity(KF.measurementMatrix);
        setIdentity(KF.processNoiseCov, cv::Scalar::all(dispersion_process));
        setIdentity(KF.measurementNoiseCov, cv::Scalar::all(dispersion_measurement));
        setIdentity(KF.errorCovPost, cv::Scalar::all(1));
        ok = 1;
        std::cout << "Constructor kalman OK" << std::endl;
    } // -- END KalmanFilter

    ~Kalman(){std::cout << "Destructor Kalman" << std::endl;}

    void work(float roll_angle, float &filteredRoll, float &rollSpeed)
    {
        measurement = roll_angle;
        KF.correct(measurement);
        prediction = KF.predict();
        filteredRoll = prediction.at<float>(0);
        rollSpeed = prediction.at<float>(1);
    } // -- END work

private:

    bool get_ini_params(const std::string& config, const std::string& block_name)
    {
        std::cout << "BEGIN get_ini_params kalman" << std::endl;
        setlocale(LC_NUMERIC, "en_US.UTF-8");
        bool configFileExists = FileIsExist(config);
        if(!configFileExists)
        {std::cout << "Config file '" << config << "' not exist!" << std::endl; return 0;}

        INIReader reader(config);
        if(reader.ParseError() < 0){std::cout << "Can't load '" << config << "'\n"; return 0;}

        dispersion_process = reader.GetReal(block_name, "dispersion_process", -1000000000);
        if(dispersion_process == -1000000000){std::cout << "dispersion_process not declared\n"; return 0;}
        std::cout << "dispersion_process = " << dispersion_process << ";\n";

        dispersion_measurement = reader.GetReal(block_name, "dispersion_measurement", -1000000000);
        if(dispersion_measurement == -1000000000){std::cout << "dispersion_measurement not declared\n"; return 0;}
        std::cout << "dispersion_measurement = " << dispersion_measurement << ";\n";

        delta_t = reader.GetReal(block_name, "delta_t", -1000000000);
        if(delta_t == -1000000000){std::cout << "delta_t not declared\n"; return 0;}
        std::cout << "delta_t = " << delta_t << ";\n";

        std::cout << "END get_ini_params Kalman" << std::endl;
        return 1;
    } // -- END get_ini_params

    bool FileIsExist(const string& filePath)
    {
        bool isExist = false;
        std::ifstream fin(filePath.c_str());
        if(fin.is_open()){isExist = true;}
        fin.close();
        return isExist;
    } // -- END FileIsExist
}; // -- END class Kalman
#endif // Kalman_H
