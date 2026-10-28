#ifndef TESTCHANNEL_H
#define TESTCHANNEL_H

#include <QObject>
#include "device_parts/channel/abstractchannel.h"

class TestChannel : public AbstractChannel
{
    Q_OBJECT
public:
    explicit TestChannel(QObject * parent = nullptr, const QString & _name = CHANNEL_NAME_TEST, const QString & _show_name = CHANNEL_SHOW_NAME_TEST, const uint32_t & _id = CHANNEL_ID_TEST, const int & _w = 1920, const int & _h = 1080);

protected:
    virtual void parse_raw_img(cv::Mat_<uint8_t> raw_img) override;

public slots:
};

#endif // TESTCHANNEL_H
