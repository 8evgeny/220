#include "testchannel.h"
#include <omp.h>

TestChannel::TestChannel(QObject * parent, const QString & name, const QString & show_name, const uint32_t & id, const int & w, const int & h):
    AbstractChannel(parent, name, show_name, id, w, h)
{
}
void TestChannel::parse_raw_img(cv::Mat_<uint8_t> raw_img)
{
    _origin_img.release();
    cv::cvtColor(raw_img, _origin_img, cv::COLOR_GRAY2BGR);
}
