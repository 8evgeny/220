#include "tvgraychannel.h"
#include <omp.h>

#include "device_parts/cmd/cmdmanager.h"

TVGrayChannel::TVGrayChannel(QObject * parent, const QString & name, const QString & show_name, const uint32_t & id, const int & w, const int & h):
    AbstractChannel(parent, name, show_name, id, w, h)
{
    //Objective
    _min_focus_mm = CHANNEL_TV_MIN_FOCUS_MM;
    _max_focus_mm = CHANNEL_TV_MAX_FOCUS_MM;
    _pixel_size_mm = CHANNEL_TV_PIX_SIZE;
    _agc_on = true;
    load();
}
void TVGrayChannel::parse_raw_img(cv::Mat_<uint8_t> raw_img)
{
    _origin_img.release();
    cv::cvtColor(raw_img, _origin_img, cv::COLOR_GRAY2BGR);
}
