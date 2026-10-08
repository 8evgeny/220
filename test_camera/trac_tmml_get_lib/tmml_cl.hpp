#pragma once
#include "CL/cl.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <fstream>
const int SOURCE_WIDTH = EXT_VAL * TEMPLATE_WIDTH;
const int WORK_WIDTH = SOURCE_WIDTH - 1;
const int WORK_AREA = WORK_WIDTH * WORK_WIDTH;
const int RESULT_WIDTH = WORK_WIDTH - TEMPLATE_WIDTH + 1;
const int RESULT_AREA = RESULT_WIDTH * RESULT_WIDTH;
const int TEMPLATE_AREA = TEMPLATE_WIDTH * TEMPLATE_WIDTH;
const float TEMPLATE_AREA_1 = 1.f / TEMPLATE_AREA;

#define KERNEL_tmml_cl_NAME1 "work_cl"

struct Pix
{
    unsigned char x = 0;
    unsigned char y = 0;
    float bright = 0;
}; // END Pix

class tmml_cl
{
  public:
    tmml_cl(bool& ok, float& min_max_Val);
    ~tmml_cl();
    void CL_FREE();
    void work_tmml(const cv::Mat& img_work, const cv::Mat& img_temp, Pix& max_pix);
    const Pix max_pix0;
    Pix max_pix = max_pix0;

  private:
    std::string kernel_source = "";
    cl::Kernel tmml_cl_kernel1, tmml_cl_kernel2;
    cl::Buffer img_work_buff, img_temp_buff, img_result_buff;
    cl::Buffer maxVal_int_buff, max_pix_buff;
    cl::CommandQueue qu;
    cl::Context context;
    cl::Program program;
    cl::Device default_device;
    std::vector<cl::Device> all_devices;
    cl::Platform default_platform;
    std::vector<cl::Platform> all_platforms;
    int maxVal_int = 0;
    void initDevice(bool& init_OK);
    void loadAndBuildProgram(bool& init_OK);
    std::string loadKernelFile(const std::string& program);
    void get_cl_string(std::string& kernel_str);
}; // END tmml_cl
