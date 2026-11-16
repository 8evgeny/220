#include "swirchannel.h"
#include <omp.h>

#include "tools/drawp.h"

SWIRChannel::SWIRChannel(QObject * parent, const QString & name, const QString & show_name, const uint32_t & id, const int & w, const int & h):
    AbstractChannel(parent, name, show_name, id, w, h)
{
    //Objective
    _min_focus_mm = CHANNEL_SWIR_MIN_FOCUS_MM;
    _max_focus_mm = CHANNEL_SWIR_MAX_FOCUS_MM;
    _pixel_size_mm = CHANNEL_SWIR_PIX_SIZE;
    _agc_on = true;
    load();
}
void SWIRChannel::parse_raw_img(cv::Mat_<uint8_t> raw_img)
{
    _origin_img.release();
    cv::cvtColor(raw_img, _origin_img, cv::COLOR_GRAY2BGR);
}
