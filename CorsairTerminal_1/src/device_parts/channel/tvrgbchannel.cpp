#include "tvrgbchannel.h"
#include <omp.h>

#include "device_parts/cmd/cmdmanager.h"

TVRGBChannel::TVRGBChannel(QObject * parent, const QString & name, const QString & show_name, const uint32_t & id, const int & w, const int & h):
    AbstractChannel(parent, name, show_name, id, w, h)
{
    //Objective
    _min_focus_mm = CHANNEL_TV_MIN_FOCUS_MM;
    _max_focus_mm = CHANNEL_TV_MAX_FOCUS_MM;
    _pixel_size_mm = CHANNEL_TV_PIX_SIZE;
    load();
}
void TVRGBChannel::parse_raw_img(cv::Mat_<uint8_t> raw_img)
{
    _origin_img.release();
    cv::demosaicing(raw_img, _origin_img, cv::COLOR_BayerGB2BGR_EA);
}
