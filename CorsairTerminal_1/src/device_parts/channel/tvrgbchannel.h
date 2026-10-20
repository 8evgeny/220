#ifndef TVRGBCHANNEL_H
#define TVRGBCHANNEL_H

#include <QObject>
#include "device_parts/channel/abstractchannel.h"

class TVRGBChannel : public AbstractChannel
{
    Q_OBJECT
public:
    explicit TVRGBChannel(QObject *parent = nullptr,
                          const QString &_name = CHANNEL_NAME_TVRGB,
                          const QString &_show_name = CHANNEL_SHOW_NAME_TVRGB,
                          const uint32_t &_id = CHANNEL_ID_TVRGB,
                          const int &w = 1920,
                          const int &h = 1080);

protected:
    virtual void parse_raw_img(cv::Mat_<uint8_t> raw_img) override;

public slots:
};

#endif // TVRGBCHANNEL_H
