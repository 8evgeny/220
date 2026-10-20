#include "tvgraymwirchannel.h"
#include <omp.h>

#include "tools/fusionalgorithm.h"

#include <QTimer>
#include <etld/etld.h>

#include "tools/logger.h"
#include <QElapsedTimer>

TVGrayMWIRChannel::TVGrayMWIRChannel(QObject * parent, const QString & name, const QString & show_name, const uint32_t & id, const int & w, const int & h):
    AbstractChannel(parent, name, show_name, id, w, h)
{
    fusionAlgorithm = new FusionAlgorithm();
    enable = false;
    method = 0;
    //Objective
    _min_focus_mm = CHANNEL_MWIR_MIN_FOCUS_MM;
    _max_focus_mm = CHANNEL_MWIR_MAX_FOCUS_MM;
    _pixel_size_mm = CHANNEL_MWIR_PIX_SIZE;
    load();
}
void TVGrayMWIRChannel::parse_raw_img(cv::Mat_<uint8_t> raw_img)
{
    _origin_img.release();
    if(enable)
    {
        cv::Mat_<uint8_t> raw_tvgray = cv::Mat_<uint8_t>(raw_img, cv::Rect(0, 0, raw_img.cols / 2, raw_img.rows)).clone();
        cv::Mat_<uint8_t> raw_mwir = cv::Mat_<uint8_t>(raw_img, cv::Rect(raw_img.cols / 2, 0, raw_img.cols / 2, raw_img.rows)).clone();
        fusionAlgorithm->fusion(raw_mwir.data, raw_tvgray.data, raw_mwir.cols, raw_mwir.rows);
        cv::cvtColor(raw_mwir, _origin_img, cv::COLOR_GRAY2BGR);
    }
    else
    {
        cv::cvtColor(raw_img, _origin_img, cv::COLOR_GRAY2BGR);
    }
}
TVGrayMWIRChannel::~TVGrayMWIRChannel()
{
    _origin_img.release();
    _display_img.release();
    delete fusionAlgorithm;
}
void TVGrayMWIRChannel::enable_changed(bool v)
{
    enable = v;
}
void TVGrayMWIRChannel::method_changed(int v)
{
    method = v;
}
void TVGrayMWIRChannel::offset_x_changed(int v)
{
    fusionAlgorithm->setXoffset(v);
}
void TVGrayMWIRChannel::offset_y_changed(int v)
{
    fusionAlgorithm->setYoffset(v);
}
void TVGrayMWIRChannel::method_agc_en_changed(bool v)
{
    fusionAlgorithm->setOutputFormat(v);
}
void TVGrayMWIRChannel::method_c_changed(double v)
{
    fusionAlgorithm->setKorrCoefC(v);
}
